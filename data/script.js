setInterval(fetchFavicon, 60000);
function fetchFavicon() {
    fetch('/favicon.ico')
        .then(response => response.blob())
        .then(icon => {
            const link = document.getElementById('icon');
            link.href = URL.createObjectURL(icon);
        })
        .catch(error => console.error('Error fetching favicon:', error));
}

setInterval(fetchData, 2000); // Fetch data every 2 seconds
function  fetchData() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            document.getElementById('temperature').innerText = data.temperature;
            document.getElementById('humidity').innerText = data.humidity;
            document.getElementById('sol_humidity').innerText = data.sol_humidity;
        })
        .catch(error => console.error('Error fetching data:', error));
}
