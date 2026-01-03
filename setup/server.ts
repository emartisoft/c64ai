const PORT = 80;

// Helper to run shell commands and get output
function run(cmd: string[]) {
  return new Deno.Command(cmd[0], {
    args: cmd.slice(1),
  }).output();
}

function delay(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

// Helper to write to a log file for debugging
async function log(message: string) {
  const timestamp = new Date().toISOString();
  try {
    await Deno.writeTextFile(
      "/home/c64user/setup.log",
      `[${timestamp}] ${message}\n`,
      { append: true },
    );
  } catch (e) {
    // If logging to the primary location fails, try a fallback.
    await Deno.writeTextFile("/tmp/setup.log", `[${timestamp}] ${message}\n`, {
      append: true,
    });
    if (e instanceof Error) {
      console.error(`Failed to write to /home/c64user/setup.log: ${e.message}`);
    } else {
      console.error(`Failed to write to /home/c64user/setup.log: ${e}`);
    }
  }
}

Deno.serve({ hostname: "0.0.0.0", port: PORT }, async (req) => {
  const url = new URL(req.url);

  // Captive portal redirect
  if (
    req.method === "GET" &&
    (url.pathname === "/generate_204" ||
      url.pathname === "/hotspot-detect.html" ||
      url.pathname === "/ncsi.txt")
  ) {
    return Response.redirect("http://10.42.0.1/", 302);
  }

  // Main page
  if (req.method === "GET" && url.pathname === "/") {
    const html = await Deno.readTextFile("./public/index.html");
    return new Response(html, {
      headers: { "content-type": "text/html" },
    });
  }

  // Wi-Fi setup
  if (req.method === "POST" && url.pathname === "/setup") {
    await log("Setup POST request received.");
    const form = await req.formData();
    const ssid = form.get("ssid")?.toString();
    const password = form.get("password")?.toString();
    const apiKey = form.get("gemini_api_key")?.toString();
    const model = form.get("gemini_model")?.toString();

    if (!ssid || !password || !apiKey || !model) {
      await log("Setup failed: Missing required fields.");
      return new Response("Missing required fields", { status: 400 });
    }

    try {
      await log(`Deleting profile existing Wi-Fi connection: preconfigured`);
      await run([
        "nmcli",
        "connection",
        "delete",
        "preconfigured",
      ]);

      //---
      await log("Disconnecting wlan0");
      await run([
        "nmcli",
        "device",
        "disconnect",
        "wlan0",
      ]);

      await run([
        "nmcli",
        "radio",
        "wifi",
        "off",
      ]);
      await delay(1000);
      await run([
        "nmcli",
        "radio",
        "wifi",
        "on",
      ]);
      await delay(2000);

      await run([
        "nmcli",
        "device",
        "wifi",
        "rescan",
      ]);
      await delay(1500);

      const list = await run([
        "nmcli",
        "-t",
        "-f",
        "SSID",
        "device",
        "wifi",
        "list",
      ]);

      const stdout = new TextDecoder().decode(list.stdout);
      const ssids = stdout
        .split("\n")
        .map((s: string) => s.trim())
        .filter(Boolean);

      if (!ssids.includes(ssid)) {
        throw new Error(
          `SSID '${ssid}' not found. Visible: ${ssids.join(", ")}`,
        );
      }

      //---

      await log(`Attempting to connect to SSID: ${ssid}`);
      const wifiConnectOutput = await run([
        "nmcli",
        "dev",
        "wifi",
        "connect",
        ssid,
        "password",
        password,
        "name",
        "preconfigured",
      ]);

      if (wifiConnectOutput.code !== 0) {
        const stderr = new TextDecoder().decode(wifiConnectOutput.stderr);
        throw new Error(
          `nmcli connect failed with code ${wifiConnectOutput.code}: ${stderr}`,
        );
      }
      await log("Successfully connected to Wi-Fi.");

      await log("Setting connection to autoconnect.");
      const autoConnectOutput = await run([
        "nmcli",
        "con",
        "modify",
        "preconfigured",
        "connection.autoconnect",
        "yes",
        "connection.autoconnect-priority",
        "10",
      ]);
      if (autoConnectOutput.code !== 0) {
        const stderr = new TextDecoder().decode(autoConnectOutput.stderr);
        throw new Error(
          `nmcli modify failed with code ${autoConnectOutput.code}: ${stderr}`,
        );
      }
      await log("Autoconnect set.");

      const envContent =
        `GEMINI_API_KEY=${apiKey}\nGEMINI_MODEL=${model}\nGEMINI_INITIAL_INSTRUCTION=Provide text-based responses. Keep the conversation context in mind. Responses should be neither too short nor too long.\nLANGUAGE=ENGLISH\n`;
      await log("Writing .env file.");
      await Deno.writeTextFile("/home/c64user/.env", envContent, {
        create: true,
      });
      await log(".env file write command issued.");

      await log("Syncing filesystem to disk.");
      await run(["sync"]);
      await log("Filesystem sync command issued.");

      setTimeout(() => {
        log("Issuing reboot command.");
        new Deno.Command("reboot").spawn();
      }, 3000); // Reduced timeout to 3s, should be enough after sync and response sent

      await log("Sending success response and scheduling reboot.");
      return new Response("Configuration saved. Device is rebooting…", {
        status: 200,
      });
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : String(err);
      await log(`FATAL ERROR in setup: ${errorMessage}`);
      console.error(err);
      return new Response(`Setup failed: ${errorMessage}`, { status: 500 });
    }
  }

  return new Response("Not Found", { status: 404 });
});

console.log("Wi-Fi setup server running on port", PORT);
