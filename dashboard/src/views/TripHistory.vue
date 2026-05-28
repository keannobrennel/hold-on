<template>
  <div class="trip-history-page">
    <div class="scroll-area">
      <div class="inner">

        <div class="stats-grid">
          <div class="stat-card">
            <div class="stat-left">
              <img src="../assets/TripsLogo.svg" alt="Trips Logo" class="stat-icon" />
              <span class="stat-label">Total Trips</span>
            </div>
            <span class="stat-value">35</span>
          </div>
          <div class="stat-card">
            <div class="stat-left">
              <img src="../assets/RiddenLogo.svg" alt="Km Ridden Logo" class="stat-icon" />
              <span class="stat-label">Km Ridden</span>
            </div>
            <span class="stat-value">124</span>
          </div>
          <div class="stat-card">
            <div class="stat-left">
              <img src="../assets/SafeLogo.svg" alt="Safe Arrivals Logo" class="stat-icon" />
              <span class="stat-label">Safe Arrivals</span>
            </div>
            <span class="stat-value">48</span>
          </div>
          <div class="stat-card">
            <div class="stat-left">
              <img src="../assets/PanicLogo.svg" alt="Panic Events Logo" class="stat-icon" />
              <span class="stat-label">Panic Events</span>
            </div>
            <span class="stat-value">5</span>
          </div>
        </div>

        <div class="trips-section">

          <div class="month-group">
            <div class="month-label">
              <span>MAY 2026</span>
              <div class="month-line"></div>
            </div>

            <div class="trip-card">
              <div class="trip-left">
                <div class="trip-route">SM City Manila → Pasay Rotonda</div>
                <div class="trip-date">May 19, 2026 • 08:00 AM</div>
                <div class="trip-badges">
                  <span class="badge danger">Panic Event</span>
                  <span class="badge danger">Not Moving</span>
                </div>
              </div>
              <div class="trip-right">
                <div class="trip-stat">
                  <span class="trip-stat-value">24:37</span>
                  <span class="trip-stat-label">Duration</span>
                </div>
                <div class="trip-stat">
                  <span class="trip-stat-value">12 KM</span>
                  <span class="trip-stat-label">Distance</span>
                </div>
              </div>
            </div>

            <div class="trip-card">
              <div class="trip-left">
                <div class="trip-route">SM City Manila → Pasay Rotonda</div>
                <div class="trip-date">May 19, 2026 • 08:00 AM</div>
                <div class="trip-badges">
                  <span class="badge success">No incidents</span>
                  <span class="badge success">Arrived Safely</span>
                </div>
              </div>
              <div class="trip-right">
                <div class="trip-stat">
                  <span class="trip-stat-value">24:37</span>
                  <span class="trip-stat-label">Duration</span>
                </div>
                <div class="trip-stat">
                  <span class="trip-stat-value">12 KM</span>
                  <span class="trip-stat-label">Distance</span>
                </div>
              </div>
            </div>
          </div>

          <div class="month-group">
            <div class="month-label">
              <span>APRIL 2026</span>
              <div class="month-line"></div>
            </div>

            <div class="trip-card">
              <div class="trip-left">
                <div class="trip-route">SM City Manila → Pasay Rotonda</div>
                <div class="trip-date">May 19, 2026 • 08:00 AM</div>
                <div class="trip-badges">
                  <span class="badge success">No incidents</span>
                  <span class="badge success">Arrived Safely</span>
                </div>
              </div>
              <div class="trip-right">
                <div class="trip-stat">
                  <span class="trip-stat-value">24:37</span>
                  <span class="trip-stat-label">Duration</span>
                </div>
                <div class="trip-stat">
                  <span class="trip-stat-value">12 KM</span>
                  <span class="trip-stat-label">Distance</span>
                </div>
              </div>
            </div>
          </div>

        </div>
      </div>
    </div>

    <div class="footer-slot"></div>
  </div>
</template>

<script>
export default {
  name: 'TripHistory',
};
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600;700;800&display=swap');

* { box-sizing: border-box; margin: 0; padding: 0; }

.trip-history-page {
  display: flex;
  flex-direction: column;
  height: 100vh;
  font-family: 'Poppins', sans-serif;
  background: #f5f5f5;
  color: #222;
  overflow: hidden;
}

.scroll-area {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 32px 24px;
}

.inner {
  width: 100%;
  max-width: 1100px;
  display: flex;
  flex-direction: column;
  gap: 32px;
}

.footer-slot { height: 72px; flex: 0 0 72px; }

/* Stats grid */
.stats-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 16px;
}

.stat-card {
  background: #fff;
  border-radius: 14px;
  padding: 18px 24px;
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: space-between;
  box-shadow: 0 4px 4px rgba(0,0,0,0.1);
  border: 1px solid #ebebeb;
}

.stat-left { display: flex; flex-direction: column; gap: 6px; }
.stat-icon { width: 24px; height: 24px; }
.stat-label { font-size: 13px; color: #888; font-weight: 500; }
.stat-value { font-size: 40px; font-weight: 600; color: #111; line-height: 1; }

/* Trips section */
.trips-section { display: flex; flex-direction: column; gap: 28px; }

.month-group { display: flex; flex-direction: column; gap: 16px; }

.month-label {
  display: flex;
  align-items: center;
  gap: 14px;
  font-size: 12px;
  font-weight: 600;
  color: #666;
  letter-spacing: 0.07em;
}

.month-label span { white-space: nowrap; }
.month-line { flex: 1; height: 1px; background: #d0d0d0; }

/* Trip card */
.trip-card {
  background: #fff;
  border-radius: 14px;
  padding: 14px 24px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  box-shadow: 0 4px 4px rgba(0,0,0,0.1);
  border: 1px solid #ebebeb;
  transition: box-shadow 0.15s;
  gap: 16px;
}

.trip-card:hover { box-shadow: 0 4px 12px rgba(0,0,0,0.12); }

.trip-left { display: flex; flex-direction: column; gap: 4px; flex: 1; min-width: 0; }

.trip-route {
  font-size: 18px;
  font-weight: 600;
  color: #111;
  line-height: 1.3;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.trip-date { font-size: 14px; color: #999; font-weight: 400; line-height: 1.3; }

.trip-badges { display: flex; gap: 8px; margin-top: 4px; flex-wrap: wrap; }

.badge {
  font-size: 12px;
  font-weight: 600;
  border-radius: 20px;
  padding: 5px 16px;
  font-family: 'Poppins', sans-serif;
  white-space: nowrap;
}

.badge.danger { background: #ffbcbb; color: #9c0000; }
.badge.success { background: #abefba; color: #339848; }

.trip-right {
  display: flex;
  gap: 48px;
  align-items: center;
  flex-shrink: 0;
}

.trip-stat { display: flex; flex-direction: column; align-items: center; gap: 2px; }
.trip-stat-value { font-size: 18px; font-weight: 600; color: #111; line-height: 1; }
.trip-stat-label { font-size: 14px; color: #bbb; font-weight: 500; }

/* ── Tablet (≤ 1024px) ── */
@media (max-width: 1024px) {
  .stats-grid { grid-template-columns: repeat(2, 1fr); }
  .stat-value { font-size: 32px; }
  .trip-right { gap: 28px; margin-right: 0; }
}

/* ── Small tablet / large mobile (≤ 768px) ── */
@media (max-width: 768px) {
  .scroll-area { padding: 20px 16px; }
  .inner { gap: 20px; }

  .stats-grid { grid-template-columns: repeat(2, 1fr); gap: 12px; }

  .stat-card { padding: 14px 16px; border-radius: 10px; }
  .stat-value { font-size: 28px; }
  .stat-label { font-size: 12px; }

  .trip-card {
    flex-direction: column;
    align-items: flex-start;
    padding: 14px 16px;
    border-radius: 10px;
  }

  .trip-right {
    width: 100%;
    justify-content: flex-start;
    gap: 32px;
    padding-top: 10px;
    border-top: 1px solid #f0f0f0;
  }

  .trip-route { font-size: 15px; white-space: normal; }
  .trip-date { font-size: 13px; }
  .trip-stat-value { font-size: 16px; }
  .trip-stat-label { font-size: 13px; }
}

/* ── Mobile (≤ 480px) ── */
@media (max-width: 480px) {
  .scroll-area { padding: 14px 12px; }
  .inner { gap: 16px; }

  .stats-grid { grid-template-columns: repeat(2, 1fr); gap: 10px; }

  .stat-card { padding: 12px 14px; border-radius: 10px; }
  .stat-value { font-size: 24px; }
  .stat-label { font-size: 11px; }
  .stat-icon { width: 20px; height: 20px; }

  .trip-card { padding: 12px 14px; gap: 12px; }

  .trip-route { font-size: 14px; }
  .trip-date { font-size: 12px; }

  .badge { font-size: 11px; padding: 4px 12px; }

  .trip-right { gap: 24px; }
  .trip-stat-value { font-size: 15px; }
  .trip-stat-label { font-size: 12px; }

  .footer-slot { height: 70px; flex: 0 0 70px; }
}
</style>