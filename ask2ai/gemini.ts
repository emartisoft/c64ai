// Import required modules and load environment variables
import { GoogleGenerativeAI } from "@google/generative-ai";
import "@std/dotenv/load";

// Get model name from environment variables
export const modelname = Deno.env.get("GEMINI_MODEL");
if (!modelname) {
  console.error("Gemini Model değişkeni ayarlanmamış!");
  Deno.exit(1);
}

// Get API key from environment variables
const key = Deno.env.get("GEMINI_API_KEY");
if (!key) {
  console.error("API_KEY ortam değişkeni ayarlanmamış!");
  Deno.exit(1);
}
export const apiKey: string = key;

export const lang = Deno.env.get("LANGUAGE");
if (!lang) {
  console.error("AI yazma dili değişkeni (LANGUAGE) ayarlanmamış!");
  Deno.exit(1);
}

// Get startup instruction from environment variables
export const initialInstruction =
  `Hangi dilde soru sorduysam hep ${lang} cevapla. ${
    Deno.env.get("GEMINI_INITIAL_INSTRUCTION")
  }`;
if (!initialInstruction) {
  console.error("Başlangıç talimatı değişkeni ayarlanmamış!");
  Deno.exit(1);
}

// Start Google Generative AI with API key
const genAI = new GoogleGenerativeAI(apiKey);

// Get manufacturer model using model name
export const model = genAI.getGenerativeModel({ model: modelname });
