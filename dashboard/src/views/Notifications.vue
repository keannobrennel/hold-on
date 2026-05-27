<template>
  <div class="notifications-page">

    <div class="layout-header-slot"></div>

    <div class="main-content">
      <aside class="sidebar">
        <div class="sidebar-header">
          <span class="sidebar-title">Quick Actions</span>
          <button class="clear-all-btn" @click="clearAll">Clear all</button>
        </div>
        <ul class="sidebar-nav">
          <li :class="['nav-item', { active: activeTab === 'all' }]" @click="activeTab = 'all'">
            <span class="nav-icon">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none"><rect x="3" y="3" width="18" height="18" rx="2" stroke="currentColor" stroke-width="2"/><path d="M3 9h18" stroke="currentColor" stroke-width="2"/></svg>
            </span>
            <span class="nav-label">All Notifications</span>
            <span class="nav-badge">{{ totalCount }}</span>
          </li>
          <li :class="['nav-item', { active: activeTab === 'panic' }]" @click="activeTab = 'panic'">
            <span class="nav-icon panic">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/><line x1="12" y1="9" x2="12" y2="13" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><line x1="12" y1="17" x2="12.01" y2="17" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>
            </span>
            <span class="nav-label">Panic Events</span>
            <span class="nav-badge">{{ panicCount }}</span>
          </li>
          <li :class="['nav-item', { active: activeTab === 'trip' }]" @click="activeTab = 'trip'">
            <span class="nav-icon trip">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none"><circle cx="12" cy="12" r="10" stroke="currentColor" stroke-width="2"/><path d="M8 12l2 2 4-4" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>
            </span>
            <span class="nav-label">Trip Updates</span>
            <span class="nav-badge">{{ tripCount }}</span>
          </li>
        </ul>
      </aside>

      <section class="notifications-panel">
        <div class="panel-header">
          <h2 class="panel-title">{{ panelTitle }}</h2>
          <button class="mark-read-btn" @click="markAllRead">
            Mark all read
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none"><circle cx="12" cy="12" r="10" stroke="#4CAF50" stroke-width="2"/><path d="M9 12l2 2 4-4" stroke="#4CAF50" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>
          </button>
        </div>

        <div class="notifications-list">
          <div v-if="todayNotifications.length" class="notif-group">
            <div class="group-label">Today</div>
            <div v-for="notif in todayNotifications" :key="notif.id" :class="['notif-card', notif.type, { unread: notif.unread }]">
              <div :class="['notif-icon-wrap', notif.type]">
                <svg v-if="notif.type === 'panic'" width="22" height="22" viewBox="0 0 24 24" fill="none"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/><line x1="12" y1="9" x2="12" y2="13" stroke="white" stroke-width="2" stroke-linecap="round"/><line x1="12" y1="17" x2="12.01" y2="17" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>
                <svg v-else width="22" height="22" viewBox="0 0 24 24" fill="none"><circle cx="12" cy="12" r="4" stroke="white" stroke-width="2"/><path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M4.93 19.07l1.41-1.41M17.66 6.34l1.41-1.41" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>
              </div>
              <div class="notif-body">
                <div class="notif-title">{{ notif.title }}</div>
                <div class="notif-desc">{{ notif.description }}</div>
                <div class="notif-time">{{ notif.time }}</div>
              </div>
              <div v-if="notif.unread" :class="['unread-dot', notif.type]"></div>
            </div>
          </div>

          <div v-if="yesterdayNotifications.length" class="notif-group">
            <div class="group-label">Yesterday</div>
            <div v-for="notif in yesterdayNotifications" :key="notif.id" :class="['notif-card', notif.type, { unread: notif.unread }]">
              <div :class="['notif-icon-wrap', notif.type]">
                <svg v-if="notif.type === 'panic'" width="22" height="22" viewBox="0 0 24 24" fill="none"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/><line x1="12" y1="9" x2="12" y2="13" stroke="white" stroke-width="2" stroke-linecap="round"/><line x1="12" y1="17" x2="12.01" y2="17" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>
                <svg v-else width="22" height="22" viewBox="0 0 24 24" fill="none"><circle cx="12" cy="12" r="4" stroke="white" stroke-width="2"/><path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M4.93 19.07l1.41-1.41M17.66 6.34l1.41-1.41" stroke="white" stroke-width="2" stroke-linecap="round"/></svg>
              </div>
              <div class="notif-body">
                <div class="notif-title">{{ notif.title }}</div>
                <div class="notif-desc">{{ notif.description }}</div>
                <div class="notif-time">{{ notif.time }}</div>
              </div>
              <div v-if="notif.unread" :class="['unread-dot', notif.type]"></div>
            </div>
          </div>
        </div>
      </section>
    </div>

    <div class="layout-footer-slot"></div>

  </div>
</template>

<script>
export default {
  name: 'NotificationsPage',
  data() {
    return {
      activeTab: 'all',
      notifications: [
        { id: 1, type: 'panic', title: 'Panic Event Triggered', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'today', unread: true },
        { id: 2, type: 'trip', title: 'Safe Zone Exit', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'today', unread: true },
        { id: 3, type: 'trip', title: 'Trip Completed', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'today', unread: true },
        { id: 4, type: 'panic', title: 'Panic Event Triggered', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'yesterday', unread: false },
        { id: 5, type: 'panic', title: 'Panic Event Triggered', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'today', unread: true },
        { id: 6, type: 'panic', title: 'Panic Event Triggered', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'yesterday', unread: false },
        { id: 7, type: 'panic', title: 'Panic Event Triggered', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'yesterday', unread: false },
        { id: 8, type: 'trip', title: 'Safe Zone Exit', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'yesterday', unread: false },
        { id: 9, type: 'trip', title: 'Trip Completed', description: 'Ana Reyes activated emergency alert near Binakayan, Kawit', time: '2 min ago', day: 'yesterday', unread: false },
      ]
    };
  },
  computed: {
    filteredNotifications() {
      if (this.activeTab === 'all') return this.notifications;
      if (this.activeTab === 'panic') return this.notifications.filter(n => n.type === 'panic');
      if (this.activeTab === 'trip') return this.notifications.filter(n => n.type === 'trip');
      return this.notifications;
    },
    todayNotifications() { return this.filteredNotifications.filter(n => n.day === 'today'); },
    yesterdayNotifications() { return this.filteredNotifications.filter(n => n.day === 'yesterday'); },
    totalCount() { return this.notifications.length + 15; },
    panicCount() { return this.notifications.filter(n => n.type === 'panic').length + 8; },
    tripCount() { return this.notifications.filter(n => n.type === 'trip').length + 6; },
    panelTitle() {
      if (this.activeTab === 'all') return 'All Notifications';
      if (this.activeTab === 'panic') return 'Panic Events';
      if (this.activeTab === 'trip') return 'Trip Updates';
      return 'All Notifications';
    }
  },
  methods: {
    markAllRead() { this.notifications.forEach(n => { n.unread = false; }); },
    clearAll() { this.notifications = []; }
  }
};
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600;700;800&display=swap');

* { box-sizing: border-box; margin: 0; padding: 0; }

.notifications-page { display: flex; flex-direction: column; height: 100vh; font-family: 'Poppins', sans-serif; background: #f5f5f5; color: #222; overflow: hidden; }

.layout-header-slot { height: 72px; flex: 0 0 72px; }

.layout-footer-slot { height: 72px; flex: 0 0 72px; }

/* Main */
.main-content { display: flex; flex: 1; overflow: hidden; padding: 24px; gap: 24px; min-height: 0; }

/* Sidebar — widened from 220px to 270px with slightly more padding */
.sidebar { width: 270px; background: #fff; border-radius: 14px; padding: 22px 20px; box-shadow: 0 4px 4px rgba(0,0,0,0.1); flex-shrink: 0; height: fit-content; }
.sidebar-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 18px; }
.sidebar-title { font-size: 15px; font-weight: 700; color: #222; }
.clear-all-btn { background: none; border: none; color: #4CAF50; font-size: 13px; cursor: pointer; font-weight: 500; font-family: 'Poppins', sans-serif; }
.sidebar-nav { list-style: none; display: flex; flex-direction: column; gap: 6px; }
.nav-item { display: flex; align-items: center; gap: 10px; padding: 11px 14px; border-radius: 10px; cursor: pointer; transition: background 0.15s; color: #555; font-size: 14px; }
.nav-item:hover { background: #f0faf0; }
.nav-item.active { background: #e8f5e9; color: #2e7d32; font-weight: 700; }
.nav-icon { display: flex; align-items: center; color: #aaa; }
.nav-item.active .nav-icon { color: #4CAF50; }
.nav-icon.panic { color: #E24B4A; }
.nav-icon.trip { color: #54FF78; }
.nav-label { flex: 1; }
.nav-badge { background: #f0f0f0; color: #555; font-size: 12px; font-weight: 600; border-radius: 20px; padding: 1px 10px; min-width: 28px; text-align: center; }
.nav-item.active .nav-badge { background: #4CAF50; color: #fff; }

/* Notifications Panel */
.notifications-panel { flex: 1; background: #fff; border-radius: 14px; padding: 24px 28px; box-shadow: 0 4px 4px rgba(0,0,0,0.1); overflow-y: auto; }
.panel-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; }
.panel-title { font-size: 22px; font-weight: 800; color: #111; }
.mark-read-btn { display: flex; align-items: center; gap: 6px; background: none; border: none; color: #4CAF50; font-size: 14px; font-weight: 500; cursor: pointer; font-family: 'Poppins', sans-serif; }

/* Groups */
.notif-group { margin-bottom: 24px; }
.group-label { font-size: 13px; color: #aaa; font-weight: 500; margin-bottom: 12px; padding-left: 2px; }

/* Cards */
.notif-card { display: flex; align-items: center; gap: 14px; padding: 14px 16px; border-radius: 12px; border: 1.5px solid #e5e5e5; margin-bottom: 10px; position: relative; background: #fff; transition: box-shadow 0.15s; }
.notif-card:hover { box-shadow: 0 2px 8px rgba(0,0,0,0.08); }
.notif-card.panic { border-color: #E24B4A; border-left-width: 4px; }
.notif-card.trip { border-color: #54FF78; border-left-width: 4px; }

/* Icon wrap */
.notif-icon-wrap { width: 44px; height: 44px; border-radius: 50%; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
.notif-icon-wrap.panic { background: #E24B4A; }
.notif-icon-wrap.trip { background: #54FF78; }

/* Body */
.notif-body { flex: 1; }
.notif-title { font-size: 15px; font-weight: 700; color: #111; margin-bottom: 2px; }
.notif-desc { font-size: 13px; color: #777; margin-bottom: 2px; }
.notif-time { font-size: 12px; color: #aaa; }

/* Unread dot */
.unread-dot { width: 10px; height: 10px; border-radius: 50%; flex-shrink: 0; }
.unread-dot.panic { background: #E24B4A; }
.unread-dot.trip { background: #4CAF50; }
</style>