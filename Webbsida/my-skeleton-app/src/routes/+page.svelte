<script lang="ts">
    import { onMount, onDestroy } from "svelte";
    import mqtt from "mqtt";
    
    let client: mqtt.MqttClient;
    
    let wActive = false;
    let aActive = false;
    let sActive = false;
    let dActive = false;
    let xActive = false;
    let cActive = false;

  //Handle key down inputs with checking keyboardevents, aswell as publishing to mqtt broker.

    function handleKeyDown(event: KeyboardEvent) {
      switch (event.key.toLowerCase()) {
        case "w":
          wActive = true;
          console.log("drive");
          if (client && client.connected) {
            client.publish("/drive", "driving");
          }
          break;
        case "a":
          aActive = true;
          console.log("Turning: Left");
          if (client && client.connected) {
            client.publish("/left", "Turning-Left");
          }
          break;
        case "s":
          sActive = true;
          console.log("Reversing");
          if (client && client.connected) {
            client.publish("/reverse", "reversing");
          }
          break;
        case "d":
          dActive = true;
          console.log("Turning-Right");
          if (client && client.connected) {
            client.publish("/right", "Turning-Right");
          }
          break;
        case "x":
          xActive = true;
          console.log("Stop");
          if (client && client.connected) {
            client.publish("/stop", "stop");
          }
          break;
        case "c":
         cActive = true;
         console.log("Centralizing")
         if (client && client.connected) {
            client.publish("/center", "centeralizing");
         }

        default:
          // Ignore other keys
          return;
      }
    }

    //Controlling in which stage keys are. Checking if key is back up and reseting values.
    
    function handleKeyUp(event: KeyboardEvent) {
      switch (event.key.toLowerCase()) {
        case "w":
          wActive = false;
          break;
        case "a":
          aActive = false;
          break;
        case "s":
          sActive = false;
          break;
        case "d":
          dActive = false;
          break;
        case "x":
          xActive = false;
          break;
        case "c":
          cActive = false;
      }
    }
    
//using onmount, making the website only active when user is focused on the window of the website.

    onMount(() => {
      // Connect to the MQTT broker
      client = mqtt.connect("ws://maqiatto.com:8883/mqtt", {
        username: "",
        password: "",
      });
    
      client.on("connect", () => {
        console.log("Connected to MQTT broker");
      });
    
      client.on("error", (error) => {
        console.error("MQTT connection error:", error);
      });
    
      window.addEventListener("keydown", handleKeyDown);
      window.addEventListener("keyup", handleKeyUp);
  
      onDestroy(() => {
      if (client) {
        client.end();
      }
      window.removeEventListener("keydown", handleKeyDown);
      window.removeEventListener("keyup", handleKeyUp);
      });
    });

    //Handling slider value changes from user, aswell as publishing the value to mqtt broker.
  
    let sliderValue = 0;
  
    function handleSliderChange(event: Event) {
      const target = event.target as HTMLInputElement;
      sliderValue = parseInt(target.value);
      console.log("Slider value:", sliderValue);
      if (client && client.connected) {
            client.publish("/speed", sliderValue.toString());
      }
    }
  </script>

  <!--style, including all the components of my website, aswell as structure-->

  <style>
    main {
      text-align: center;
      padding: 1em;
      max-width: 240px;
      margin: 0 auto;
      position: relative;
    }
    h1 {
      color: #ff3e00;
      text-transform: uppercase;
      font-size: 2em;
    }
    .space {
      justify-content: center;
      align-items: center;
      height: 200px;
    }
    .container {
      display: grid;
      grid-template-rows: repeat(3, 100px);
      grid-template-columns: repeat(3, 100px);
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    .spacehorizontal{
        justify-content: center;
        align-items: center;
        width: 300px;
    }
    .button {
      width: 100px;
      height: 100px;
      display: flex;
      justify-content: center;
      align-items: center;
      font-size: 24px;
      cursor: pointer;
      border: 2px solid black;
      user-select: none;
      background-color: black;
    }
    .active {
      background-color: rgb(255, 255, 255);
    }
    .slider-container {
      position: absolute;
      top: 35%;
      right: -250px;
      width: 30px;
      height: 300px;
      display: flex;
      flex-direction: column-reverse;
      align-items: right;
    }
    .slider {
      writing-mode: bt-lr; 
      appearance: slider-vertical; 
      -webkit-appearance: slider-vertical; 
      width: 100%;
      height: 100%;
    }
    .slider-value {
      margin-top: 10px;
      font-size: 18px;
      text-align: center;
    }
  </style>
  
  
  <main>
    <h1>Driverbot controller</h1>
    <div class="space"></div>
    <div class="container">
      <div></div>
      <div class="button {wActive ? 'active' : ''}">W</div>
      <div></div>
      <div class="button {aActive ? 'active' : ''}">A</div>
      <div class="button {sActive ? 'active' : ''}">S</div>
      <div class="button {dActive ? 'active' : ''}">D</div>
      <div></div>
      <div class="button {xActive ? 'active' : ''}">STOP=x</div>
      <div class="button {cActive ? 'active' : ''}">CENTER=c</div>
    </div>
    <div class="spacehorizontal"></div>
    <div class="slider-container">
      <input type="range" min="0" max="255" value={sliderValue} on:change={handleSliderChange} class="slider">
      <div class="slider-value">{sliderValue}</div>
    </div>
  </main>