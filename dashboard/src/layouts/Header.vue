<template>
  <header :style="headerStyle">
    <!-- Left: Logo -->
    <div :style="logoSectionStyle">
      <img
        :src="logoSrc"
        alt="HoldON Logo"
        :style="logoImgStyle"
      />
    </div>

    <!-- Center: Trip Info -->
    <div :style="tripSectionStyle">
      <span :style="tripNumberStyle">
        TRIP # {{ tripNumber }}
      </span>

      <span :style="tripTimeStyle">
        started at {{ liveTime }}
      </span>
    </div>

    <!-- Right: Exit Button -->
    <div :style="exitSectionStyle">
      <button
        :style="exitButtonStyle"
        @mouseenter="onHover"
        @mouseleave="onLeave"
        @click="$emit('exit')"
      >
        Exit

        <svg
          xmlns="http://www.w3.org/2000/svg"
          width="15"
          height="15"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          stroke-width="2"
          stroke-linecap="round"
          stroke-linejoin="round"
          :style="{ marginLeft: '6px' }"
        >
          <path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4" />
          <polyline points="16 17 21 12 16 7" />
          <line x1="21" y1="12" x2="9" y2="12" />
        </svg>
      </button>
    </div>
  </header>
</template>

<script>
import holdonLogo from "../assets/holdon_logo.png";

export default {
  name: "TripHeader",

  emits: ["exit"],

  props: {
    logoSrc: {
      type: String,
      default: holdonLogo,
    },

    tripNumber: {
      type: [String, Number],
      default: "12345",
    },
  },

  data() {
    return {
      isHovered: false,
      liveTime: "",
      interval: null,
    };
  },

  mounted() {
    this.updateTime();

    this.interval = setInterval(() => {
      this.updateTime();
    }, 1000);
  },

  beforeUnmount() {
    clearInterval(this.interval);
  },

  computed: {
    headerStyle() {
      return {
        position: "fixed",
        top: "0",
        left: "0",
        width: "100%",
        height: "48px",
        zIndex: "100",

        display: "flex",
        alignItems: "center",
        justifyContent: "space-between",

        padding: "0 16px",

        backgroundColor: "#f5f5f5",
        borderBottom: "1px solid #dcdcdc",

        boxSizing: "border-box",
        fontFamily: "'Inter', sans-serif",
      };
    },

    logoSectionStyle() {
      return {
        flex: "1",
        display: "flex",
        alignItems: "center",
      };
    },

    logoImgStyle() {
      return {
        height: "28px",
        width: "auto",
        objectFit: "contain",
      };
    },

    tripSectionStyle() {
      return {
        flex: "1",
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        justifyContent: "center",
        lineHeight: "1.1",
      };
    },

    tripNumberStyle() {
      return {
        fontSize: "15px",
        fontWeight: "700",
        color: "#339848",
        textTransform: "uppercase",
        letterSpacing: "0.02em",
        fontFamily: "'Inter', sans-serif",
      };
    },

    tripTimeStyle() {
      return {
        fontSize: "12px",
        fontWeight: "600",
        color: "#1E1E1E",
        marginTop: "2px",
        fontFamily: "'Inter', sans-serif",
      };
    },

    exitSectionStyle() {
      return {
        flex: "1",
        display: "flex",
        justifyContent: "flex-end",
        alignItems: "center",
      };
    },

    exitButtonStyle() {
      return {
        display: "flex",
        alignItems: "center",
        justifyContent: "center",

        padding: "7px 20px",

        fontSize: "14px",
        fontWeight: "700",

        color: this.isHovered ? "#fff" : "#9C0000",
        backgroundColor: this.isHovered ? "#9C0000" : "#FFC3C2",

        border: "1px solid #9C0000",
        borderRadius: "10px",

        cursor: "pointer",
        transition: "all 0.2s ease",

        outline: "none",
        fontFamily: "'Inter', sans-serif",
      };
    },
  },

  methods: {
    onHover() {
      this.isHovered = true;
    },

    onLeave() {
      this.isHovered = false;
    },

    updateTime() {
      const now = new Date();

      this.liveTime = now
        .toLocaleTimeString("en-US", {
          hour: "numeric",
          minute: "2-digit",
          hour12: true,
        })
        .replace("am", "AM")
        .replace("pm", "PM");
    },
  },
};
</script>

<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@100..900&display=swap');
</style>