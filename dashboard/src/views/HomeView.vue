<template>
  <div class="home-page">
    <h1 class="page-title">Welcome Home</h1>
    <p class="page-subtitle">Header and footer are live — start building your content here.</p>
  </div>
</template>

<style scoped>
.home-page {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  min-height: 60vh;
  gap: 1rem;
  text-align: center;
}

.page-title {
  font-size: 2rem;
  font-weight: 700;
  color: #1a1a2e;
  margin: 0;
}

.page-subtitle {
  font-size: 1rem;
  color: #6b7280;
  margin: 0;
}
</style>