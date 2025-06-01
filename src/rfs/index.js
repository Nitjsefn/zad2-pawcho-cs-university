console.log("Hello from js");

var map = L.map('map').setView([51.246452, 22.568445], 13);

let marker = L.marker([51.246452, 22.568445]);
marker.addTo(map);

let latlng = null;

L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(map);

map.on("click", (ev) => {
    marker.setLatLng([ev.latlng.lat, ev.latlng.lng]);
});

async function reqWeather()
{
    const lat = marker.getLatLng().lat;
    const lng = marker.getLatLng().lng;
    const res = await fetch(`https://api.open-meteo.com/v1/forecast?latitude=${lat}&longitude=${lng}&current=temperature_2m,wind_speed_10m`);
    const json = await res.json();
    document.getElementById("temp").innerText = json.current.temperature_2m;
    document.getElementById("temp_u").innerText = json.current_units.temperature_2m;
    document.getElementById("wind").innerText = json.current.wind_speed_10m;
    document.getElementById("wind_u").innerText = json.current_units.wind_speed_10m;
}
