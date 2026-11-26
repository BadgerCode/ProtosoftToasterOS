import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/HomeView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  // Go to the top of each new page, to #headings in the guides (below the sticky nav bar),
  // and back to where you were with the browser's back button
  scrollBehavior(to, _from, savedPosition) {
    if (savedPosition) return savedPosition
    if (to.hash) return { el: to.hash, top: 72 }
    return { top: 0 }
  },
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView,
    },
    {
      path: '/guides',
      name: 'guides',
      component: () => import('@/views/GuidesView.vue'),
    },
    {
      // e.g. /guides/toaster-os/setting-up
      path: '/guides/:slug(.*)',
      name: 'guide',
      component: () => import('@/views/GuideView.vue'),
      props: true,
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
