"use strict";
const fetch = require('node-fetch');
const express = require('express');
const IP = require('ip');
const app = express()
const port = 5500
const path = require("path")
//Note: index.html is in the same directory as the server.js file
let testPath = __dirname;
let data;
app.use(express.static(testPath));
app.get('/weather/:city/:country/:type', getData)
app.get('/pollution/:lat/:long/:type', getData)
app.get('/getIP', getIP)
app.listen(port, () => console.log(`Example app listening on port ${port}!`))

async function getData(req, res) {
    let type = req.params.type;
    let arg1;
    let arg2;
    if (type == "weather") {
        arg1 = req.params.city;
        arg2 = req.params.country;
    }
    else if (type == "pollution") {
        arg1 = req.params.lat;
        arg2 = req.params.long;
    }
    const response = await retrieveData(arg1, arg2, type);
    res.json(data);
}

async function retrieveData(arg1, arg2, type) {
    const id = ""; //API key would be here
    let str = "";
    if (type == "weather") {
        str = `http://api.openweathermap.org/data/2.5/forecast?q=${arg1},${arg2}&units=metric&APPID=${id}`;
    }
    else if (type == "pollution") {
        str = `http://api.openweathermap.org/data/2.5/air_pollution/forecast?lat=${arg1}&lon=${arg2}&units=metric&APPID=${id}`;
    }
    const response = await fetch(str);
    data = await response.json();
}

async function getIP(req, res) {
    const ip = IP.address();
    const response = await retrieveIP();
    res.json(data);
}

async function retrieveIP() {
    const id = ""; //API key would be here
    const str = `https://ipgeolocation.abstractapi.com/v1/?api_key=${id}`;
    const response = await fetch(str);
    data = await response.json();
}