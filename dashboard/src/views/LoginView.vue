import { createRouter, createWebHistory } from "vue-router";
import { auth } from "../firebase";
import { onAuthStateChanged } from "firebase/auth";

import LoginView from "../views/LoginView.vue";
import HomeView from "../views/HomeView/HomeView.vue";
import SharedView from "../views/SharedView/SharedView.vue";
import NotificationsView from "../views/Notifications.vue";
import TripHistoryView from "../views/TripHistory.vue";
import UserLayout from "../layouts/UserLayout.vue";

const routes = [
  {
    path: "/",
    redirect: "/login",
  },
  {
    path: "/login",
    name: "Login",
    component: LoginView,
  },
  {
    path: "/app",
    component: UserLayout,
    children: [
      {
        path: "home",
        name: "Home",
        component: HomeView,
        meta: { requiresAuth: true },
      },
      {
        path: "notifications",
        name: "Notifications",
        component: NotificationsView,
        meta: { requiresAuth: true },
      },
      {
        path: "trip-history",
        name: "TripHistory",
        component: TripHistoryView,
        meta: { requiresAuth: true },
      },
      {
        // Public — no requiresAuth, anyone with the link can view
        path: "/trip/:tripId",
        name: "SharedTrip",
        component: SharedView,
      },
    ],
  },
  {
    path: "/:pathMatch(.*)*",
    redirect: "/login",
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

const getCurrentUser = () =>
  new Promise((resolve) => {
    const unsub = onAuthStateChanged(auth, (user) => {
      unsub();
      resolve(user);
    });
  });

router.beforeEach(async (to) => {
  if (!to.meta.requiresAuth) return true;

  const user = await getCurrentUser();

  console.log("Guard fired for:", to.path, "User:", user);

  if (!user) {
    return { path: "/login" };
  }

  return true;
});

export default router;