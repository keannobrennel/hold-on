import { createRouter, createWebHistory } from "vue-router";
import { auth } from "../firebase";
import { onAuthStateChanged } from "firebase/auth";

import LoginView from "../views/LoginView.vue";
import HomeView from "../views/HomeView.vue";
import TripView from "../views/TripView.vue";

import UserLayout from "../layouts/UserLayout.vue";

const routes = [
  {
    path: "/",
    redirect: "/home",
  },

  {
    path: "/login",
    name: "Login",
    component: LoginView,
  },

  {
    path: "/",
    component: UserLayout,
    meta: { requiresAuth: true },

    children: [
      {
        path: "home",
        name: "Home",
        component: HomeView,
      },

      {
        path: "trip/:tripId",
        name: "Trip",
        component: TripView,
      },
    ],
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

// Wait for Firebase auth state
const getCurrentUser = () =>
  new Promise((resolve) => {
    const unsub = onAuthStateChanged(auth, (user) => {
      unsub();
      resolve(user);
    });
  });

router.beforeEach(async (to) => {
  if (to.meta.requiresAuth) {
    const user = await getCurrentUser();

    if (!user) {
      return "/login";
    }
  }
});

export default router;