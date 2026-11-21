import { reactive } from "vue";

export const appStore = reactive({
  example: "Example state" as string | null
});
