import { createRouter, createWebHistory } from "vue-router";
import { auth } from "../firebase";

import LoginView from "../views/LoginView.vue";
import HomeView from "../views/HomeView.vue";
import TripView from "../views/TripView.vue";
import HardwareTest from "../views/testing-ui/HardwareTest2.vue";

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
    path: "/home",
    name: "Home",
    component: HomeView,
    meta: { requiresAuth: true },
  },
  {
    path: "/trip/:tripId",
    name: "Trip",
    component: TripView,
    meta: { requiresAuth: true },
  },
  {
    path: "/hardware-test",
    name: "HardwareTest",
    component: HardwareTest,
    // no requiresAuth for now, so you can test ESP without logging in
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

router.beforeEach((to, from, next) => {
  const user = auth.currentUser;

  if (to.meta.requiresAuth && !user) {
    next("/login");
    return;
  }

  next();
});

export default router;