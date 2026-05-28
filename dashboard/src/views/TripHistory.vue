<template>
  <div
    style="
      display: flex;
      flex-direction: column;
      height: 100vh;
      font-family: &quot;Poppins&quot;, sans-serif;
      background: #f5f5f5;
      color: #222;
      overflow: hidden;
      box-sizing: border-box;
    "
  >
    <div style="height: 72px; flex: 0 0 72px"></div>

    <div
      style="
        flex: 1;
        overflow-y: auto;
        display: flex;
        flex-direction: column;
        align-items: center;
        padding: 32px 24px;
      "
    >
      <div
        style="
          width: 100%;
          max-width: 1100px;
          display: flex;
          flex-direction: column;
          gap: 32px;
        "
      >
        <div
          style="
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 16px;
          "
        >
          <div
            style="
              background: #fff;
              border-radius: 14px;
              padding: 18px 24px;
              display: flex;
              flex-direction: row;
              align-items: center;
              justify-content: space-between;
              box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
              border: 1px solid #ebebeb;
            "
          >
            <div style="display: flex; flex-direction: column; gap: 6px">
              <img
                src="../assets/TripsLogo.svg"
                alt="Trips Logo"
                style="width: 24px; height: 24px"
              />
              <span style="font-size: 13px; color: #888; font-weight: 500"
                >Total Trips</span
              >
            </div>
            <span
              style="
                font-size: 40px;
                font-weight: 600;
                color: #111;
                line-height: 1;
              "
              >35</span
            >
          </div>

          <div
            style="
              background: #fff;
              border-radius: 14px;
              padding: 18px 24px;
              display: flex;
              flex-direction: row;
              align-items: center;
              justify-content: space-between;
              box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
              border: 1px solid #ebebeb;
            "
          >
            <div style="display: flex; flex-direction: column; gap: 6px">
              <img
                src="../assets/RiddenLogo.svg"
                alt="Km Ridden Logo"
                style="width: 24px; height: 24px"
              />
              <span style="font-size: 13px; color: #888; font-weight: 500"
                >Km Ridden</span
              >
            </div>
            <span
              style="
                font-size: 40px;
                font-weight: 600;
                color: #111;
                line-height: 1;
              "
              >124</span
            >
          </div>

          <div
            style="
              background: #fff;
              border-radius: 14px;
              padding: 18px 24px;
              display: flex;
              flex-direction: row;
              align-items: center;
              justify-content: space-between;
              box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
              border: 1px solid #ebebeb;
            "
          >
            <div style="display: flex; flex-direction: column; gap: 6px">
              <img
                src="../assets/SafeLogo.svg"
                alt="Safe Arrivals Logo"
                style="width: 24px; height: 24px"
              />
              <span style="font-size: 13px; color: #888; font-weight: 500"
                >Safe Arrivals</span
              >
            </div>
            <span
              style="
                font-size: 40px;
                font-weight: 600;
                color: #111;
                line-height: 1;
              "
              >48</span
            >
          </div>

          <div
            style="
              background: #fff;
              border-radius: 14px;
              padding: 18px 24px;
              display: flex;
              flex-direction: row;
              align-items: center;
              justify-content: space-between;
              box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
              border: 1px solid #ebebeb;
            "
          >
            <div style="display: flex; flex-direction: column; gap: 6px">
              <img
                src="../assets/PanicLogo.svg"
                alt="Panic Events Logo"
                style="width: 24px; height: 24px"
              />
              <span style="font-size: 13px; color: #888; font-weight: 500"
                >Panic Events</span
              >
            </div>
            <span
              style="
                font-size: 40px;
                font-weight: 600;
                color: #111;
                line-height: 1;
              "
              >5</span
            >
          </div>
        </div>

        <div style="display: flex; flex-direction: column; gap: 28px">
          <div style="display: flex; flex-direction: column; gap: 16px">
            <div
              style="
                display: flex;
                align-items: center;
                gap: 14px;
                font-size: 12px;
                font-weight: 600;
                color: #666;
                letter-spacing: 0.07em;
              "
            >
              <span style="white-space: nowrap">MAY 2026</span>
              <div style="flex: 1; height: 1px; background: #d0d0d0"></div>
            </div>

            <div
              style="
                background: #fff;
                border-radius: 14px;
                padding: 14px 24px;
                display: flex;
                align-items: center;
                justify-content: space-between;
                box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
                border: 1px solid #ebebeb;
                transition: box-shadow 0.15s;
              "
            >
              <div style="display: flex; flex-direction: column; gap: 4px">
                <div
                  style="
                    font-size: 18px;
                    font-weight: 600;
                    color: #111;
                    line-height: 1.3;
                  "
                >
                  SM City Manila → Pasay Rotonda
                </div>
                <div
                  style="
                    font-size: 14px;
                    color: #999;
                    font-weight: 400;
                    line-height: 1.3;
                  "
                >
                  May 19, 2026 • 08:00 AM
                </div>
                <div style="display: flex; gap: 8px; margin-top: 4px">
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #ffbcbb;
                      color: #9c0000;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >Panic Event</span
                  >
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #ffbcbb;
                      color: #9c0000;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >Not Moving</span
                  >
                </div>
              </div>
              <div
                style="
                  display: flex;
                  gap: 48px;
                  align-items: center;
                  flex-shrink: 0;
                  margin-right: 60px;
                "
              >
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >24:37</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Duration</span
                  >
                </div>
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >12 KM</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Distance</span
                  >
                </div>
              </div>
            </div>

            <div
              style="
                background: #fff;
                border-radius: 14px;
                padding: 14px 24px;
                display: flex;
                align-items: center;
                justify-content: space-between;
                box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
                border: 1px solid #ebebeb;
              "
            >
              <div style="display: flex; flex-direction: column; gap: 4px">
                <div
                  style="
                    font-size: 18px;
                    font-weight: 600;
                    color: #111;
                    line-height: 1.3;
                  "
                >
                  SM City Manila → Pasay Rotonda
                </div>
                <div
                  style="
                    font-size: 14px;
                    color: #999;
                    font-weight: 400;
                    line-height: 1.3;
                  "
                >
                  May 19, 2026 • 08:00 AM
                </div>
                <div style="display: flex; gap: 8px; margin-top: 4px">
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #abefba;
                      color: #339848;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >No incidents</span
                  >
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #abefba;
                      color: #339848;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >Arrived Safely</span
                  >
                </div>
              </div>
              <div
                style="
                  display: flex;
                  gap: 48px;
                  align-items: center;
                  flex-shrink: 0;
                  margin-right: 60px;
                "
              >
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >24:37</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Duration</span
                  >
                </div>
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >12 KM</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Distance</span
                  >
                </div>
              </div>
            </div>
          </div>

          <div style="display: flex; flex-direction: column; gap: 16px">
            <div
              style="
                display: flex;
                align-items: center;
                gap: 14px;
                font-size: 12px;
                font-weight: 600;
                color: #666;
                letter-spacing: 0.07em;
              "
            >
              <span style="white-space: nowrap">APRIL 2026</span>
              <div style="flex: 1; height: 1px; background: #d0d0d0"></div>
            </div>

            <div
              style="
                background: #fff;
                border-radius: 14px;
                padding: 14px 24px;
                display: flex;
                align-items: center;
                justify-content: space-between;
                box-shadow: 0 4px 4px rgba(0, 0, 0, 0.1);
                border: 1px solid #ebebeb;
              "
            >
              <div style="display: flex; flex-direction: column; gap: 4px">
                <div
                  style="
                    font-size: 18px;
                    font-weight: 600;
                    color: #111;
                    line-height: 1.3;
                  "
                >
                  SM City Manila → Pasay Rotonda
                </div>
                <div
                  style="
                    font-size: 14px;
                    color: #999;
                    font-weight: 400;
                    line-height: 1.3;
                  "
                >
                  May 19, 2026 • 08:00 AM
                </div>
                <div style="display: flex; gap: 8px; margin-top: 4px">
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #abefba;
                      color: #339848;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >No incidents</span
                  >
                  <span
                    style="
                      font-size: 12px;
                      font-weight: 600;
                      border-radius: 20px;
                      padding: 5px 16px;
                      background: #abefba;
                      color: #339848;
                      font-family: &quot;Poppins&quot;, sans-serif;
                    "
                    >Arrived Safely</span
                  >
                </div>
              </div>
              <div
                style="
                  display: flex;
                  gap: 48px;
                  align-items: center;
                  flex-shrink: 0;
                  margin-right: 60px;
                "
              >
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >24:37</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Duration</span
                  >
                </div>
                <div
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    gap: 2px;
                  "
                >
                  <span
                    style="
                      font-size: 18px;
                      font-weight: 600;
                      color: #111;
                      line-height: 1;
                    "
                    >12 KM</span
                  >
                  <span style="font-size: 14px; color: #bbb; font-weight: 500"
                    >Distance</span
                  >
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div style="height: 72px; flex: 0 0 72px"></div>
  </div>
</template>

<script>
export default {
  name: "TripHistory",
};
</script>
