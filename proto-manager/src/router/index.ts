import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/HomeView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView,
    },
    {
      path: '/configure',
      component: () => import('@/views/ConfigureView.vue'),
      children: [
        { path: '', redirect: { name: 'configure-panels' } },
        {
          path: 'panels',
          name: 'configure-panels',
          component: () => import('@/views/configure/PanelLayoutView.vue'),
        },
        {
          path: 'expressions',
          name: 'configure-expressions',
          component: () => import('@/views/configure/ExpressionsView.vue'),
        },
        {
          path: 'features',
          name: 'configure-features',
          component: () => import('@/views/configure/FeaturesView.vue'),
        },
      ],
    },
  ],
})

export default router
