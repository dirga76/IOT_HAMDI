const red = document.querySelector('#red');
const yellow = document.querySelector('#yellow');
const green = document.querySelector('#green');

var connection = new WebSocket('ws://' + location.hostname + ':81/')
var btn = document.getElementById('btn')
var btn1 = document.getElementById('btn1')

var title = 0;
var counter = 0;

connection.onmessage = function(event) {
    var full_data = event.data
    console.log(full_data)
    var data = JSON.parse(full_data)

    title = data.title
    counter = data.counter

    document.getElementById("title_value").innerHTML = title
    document.getElementById("counter_value").innerHTML = counter
}

function leftClick() {
    btn.style.left = '0'
    green.classList.remove('active')
    send_data()
}

function rightClick() {
    btn.style.left = '112px'
    green.classList.add('active')
    send_data()
}

function left() {
    btn1.style.left = '0'
    document.getElementById('btn');
    send_data()
}

function right() {
    btn1.style.left = '112px'
    if (supplyVoltage >= 100) {
        document.getElementById("tank").src = "Low.png";
        red.classList.add('active')
        green.classList.add('active')
        yellow.classList.remove('active')
        send_data()
    } else {
        document.getElementById("tank").src = "Hight.png";
        red.classList.remove('active')
        green.classList.remove('active')
        yellow.classList.add('active')
        send_data()
    }
}

function send_data() {
    var full_data = '{"emon1" :' + btn1 + ',"Kontak":' + btn + '}'
    connection.send(full_data)
}