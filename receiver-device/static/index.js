const contentBg = document.querySelector(".content");
const statusText = document.querySelector(".status");
const iconText = document.querySelector(".icon");
const doorbellButton = document.querySelector(".doorbell");
const modeInput = document.querySelector(".mode");

const evt = new CustomEvent("change");

setTimeout(() => {
  modeInput.value = 5;
  modeInput.dispatchEvent(evt);
}, 3000);

const randomHotelNo = Math.floor(Math.random() * (520 - 500 + 1)) + 500;

const showDoorbell = (isHidden) => {
  if (!isHidden) {
    doorbellButton.style.display = "flex";
  } else {
    doorbellButton.style.display = "none";
  }
};

const template = (background, text, icon) => {
  contentBg.style.background = background;
  statusText.textContent = text;
  iconText.textContent = icon;
};

const ringDoorBell = () => {
  console.log("Doorbell rung");
};

setMode = (idx) => {
  switch (idx) {
    case 0:
      // Default Screen
      template("#263238", "room " + randomHotelNo, "🔓");
      break;
    case 1:
      // Access Granted
      template("#4CAF50", "access granted", "🔓");
      break;
    case 2:
      // Access Denied
      template("#B00020", "access denied", "🔓");
      break;
    case 3:
      // Do Not Disturb
      template("#F9A825", "do not disturb", "🔓");
      break;
    case 4:
      // Make Up Room
      template("#039BE5", "make up room", "🔓");
      break;
    default:
      // Default Screen
      template("#263238", "room " + randomHotelNo, "🔓");
      break;
  }
};

setInterval(() => {
  fetch("http://127.0.0.1:5000/status")
  .then(response => response.json())
  .then(data => {
    console.log(data.status)
    setMode(Number(data.status))
  })
}, 500);