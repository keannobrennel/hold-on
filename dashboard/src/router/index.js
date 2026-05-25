import { createRouter, createWebHistory } from "vue-router";
import { auth } from "../firebase";

import LoginView from "../views/LoginView.vue";
import HomeView from "../views/HomeView.vue";
import TripView from "../views/TripView.vue";
import SharedView from "../views/SharedView.vue";

const routes = [{
        path: "/",
        redirect: "/login",
    },
    {
        path: "/login",
        name: "Login",
        component: LoginView,
    },
    {
        path: "/sharedview",
        name: "SharedView",
        component: SharedView,
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
    },
];

const router = createRouter({
    history: createWebHistory(),
    routes,
});

// Guard: redirect to login if not authenticated
router.beforeEach((to, from, next) => {
    const user = auth.currentUser;
    if (to.meta.requiresAuth && !user) {
        next("/login");
    } else {
        next();
    }
});

export default router;