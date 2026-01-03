// ask2ai.ts
// Coded by emarti, Murat Özdemir
// This code can only be run with 'deno' (https://deno.com/)
// deno run -A ask2ai.ts

import { initialInstruction, model } from "./gemini.ts";
import wrapAnsi from "wrap-ansi";

const chatHistory: string[] = [];
// On C64, data is written in rows of 33 columns.
const columnWidth = 33;

// Build prompt with history
function buildPrompt(history: string[]): string {
  return `${initialInstruction}\n${history.join("\n")}\n`;
}

function sanitizeForAscii(text: string): string {
  const replacements: { [key: string]: string } = {
    "Ö": "\xA3",
    "ö": "\xA2",
    "Ç": "\xA1",
    "ç": "\xBB",
    "Ş": "\xA7",
    "ş": "\xA6",
    "\u0130": "\xAA", // İ
    "\u0131": "\xAB", // ı
    "Ğ": "\xA5",
    "ğ": "\xA4",
    "Ü": "\xA9",
    "ü": "\xA8",
    "`": "'",
    "{": "\xAD",
    "}": "\xAE",
    // German characters
    "ß": "\xAF",
    "ẞ": "\xAF",
    // Azeri characters
    "ə": "\xB0",
    "Ə": "\xB1",
    "¿": "\xBC",
    "¡": "\xBD",
    "|": "\xBE",
  };

  const replacements2: { [key: string]: string } = {
    // Western European languages
    "á": "a",
    "à": "a",
    "â": "a",
    "ä": "a",
    "ã": "a",
    "å": "a",
    "Á": "A",
    "À": "A",
    "Â": "A",
    "Ä": "A",
    "Ã": "A",
    "Å": "A",

    "é": "e",
    "è": "e",
    "ê": "e",
    "ë": "e",
    "É": "E",
    "È": "E",
    "Ê": "E",
    "Ë": "E",

    "í": "i",
    "ì": "i",
    "î": "i",
    "ï": "i",
    "Í": "I",
    "Ì": "I",
    "Î": "I",
    "Ï": "I",

    "ó": "o",
    "ò": "o",
    "ô": "o",
    "õ": "o",
    "Ó": "O",
    "Ò": "O",
    "Ô": "O",
    "Õ": "O",

    "ú": "u",
    "ù": "u",
    "û": "u",
    "Ú": "U",
    "Ù": "U",
    "Û": "U",

    "ñ": "n",
    "Ñ": "N",

    "œ": "oe",
    "Œ": "OE",
    "æ": "ae",
    "Æ": "AE",
    "ÿ": "y",
    "Ÿ": "Y",
  };

  // Perform specific Turkish & other character replacements
  let sanitized = text.replace(
    /[ÖöÇçŞşİıĞğÜü{}ẞßəƏ¿¡|]/g,
    (char) => replacements[char] || char,
  );

  // Perform specific Western European languages character replacements
  sanitized = sanitized
    .split("")
    .map((c) => replacements2[c] ?? c)
    .join("");

  // Clear non-ASCII characters (0x20-0xEF) and newline (0x0d, 0x0a)
  sanitized = sanitized.replace(/[^\x20-\xEF\n\r]/g, " ");

  let out = "";
  let i = 0;
  let toggle = true;

  // Reverse the code line header formatted ```
  while (i < sanitized.length) {
    if (
      sanitized[i] === "`" &&
      sanitized[i + 1] === "`" &&
      sanitized[i + 2] === "`"
    ) {
      out += toggle ? "\x12" : "\x92";
      toggle = !toggle;
      i += 3;
    } else {
      out += sanitized[i];
      i++;
    }
  }

  // Perform specific ` character replacements
  out = out.replace(
    /[`]/g,
    (char) => replacements[char] || char,
  );

  out = out.replace(/\*{2,}/g, "");

  return out;
}

(async () => {
  try {
    const historyContent = await Deno.readTextFile("@history.txt");
    if (historyContent) {
      // Filter out empty lines from the history file
      chatHistory.push(
        ...historyContent.split("\n").filter((line) => line.trim() !== ""),
      );
    }
  } catch (error) {
    if (!(error instanceof Deno.errors.NotFound)) {
      console.error("file not read:", error);
    }
    // If the file doesn't exist, we just start with an empty history.
  }

  const userInput = await Deno.readTextFile("@prompttext.txt");
  chatHistory.push(userInput);

  const promptText = buildPrompt(chatHistory);

  try {
    const result = await model.generateContent([promptText]);
    const response = await result.response;
    const assistantReply = await response.text();

    // Sanitize the reply for C64 compatibility
    const sanitizedReply = sanitizeForAscii(assistantReply);

    // Swap the case of the text
    const caseSwappedReply = sanitizedReply.split("").map((char) => {
      if (char >= "a" && char <= "z") {
        return char.toUpperCase();
      } else if (char >= "A" && char <= "Z") {
        return char.toLowerCase();
      }
      return char;
    }).join("");

    // Write the final text to the response file for the C64
    let finalText = wrapAnsi(caseSwappedReply, columnWidth);
    if (finalText.trim() === "") {
      finalText = "???";
    }
    const bytes = new Uint8Array(finalText.length);
    for (let i = 0; i < finalText.length; i++) {
      bytes[i] = finalText.charCodeAt(i);
    }
    await Deno.writeFile("@response.txt", bytes);

    // Add the new assistant reply to the history for the next prompt
    chatHistory.push(assistantReply);

    // Save the new user input and assistant reply to the history file
    const historyToSave = `${userInput}\n${assistantReply}\n`;
    await Deno.writeTextFile("@history.txt", historyToSave, { append: true });
  } catch (error) {
    console.error("error:", error);
    await Deno.writeTextFile(
      "@response.txt",
      "rESPONSE COULD NOT BE RETRIEVED,\nPLEASE TRY AGAIN.",
    );
  }
})();
