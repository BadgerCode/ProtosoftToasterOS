import '@fontsource-variable/doto/full.css'
import '@fontsource-variable/atkinson-hyperlegible-next'
import './assets/main.css'
import { createApp } from 'vue'
import App from './App.vue'
import router from './router'

const app = createApp(App)

app.use(router)

app.mount('#app')
