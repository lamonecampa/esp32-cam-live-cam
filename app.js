// =========================
// ESP32 DASHBOARD API
// =========================

const API = {

    status:'/status',
    stream:'/stream',
    snapshot:'/snapshot',
    files:'/files',
    config:'/config',
    ota:'/ota',
    ai:'/ai',
    cloud:'/cloud',
    recordStart:'/recordStart',
    recordStop:'/recordStop'

};

// =========================
// ELEMENTS
// =========================

const sidebar=document.getElementById('sidebar');
const menuToggle=document.getElementById('menuToggle');
const overlay=document.getElementById('overlay');
const cameraStream=document.getElementById('cameraStream');

// =========================
// SIDEBAR
// =========================

menuToggle?.addEventListener('click',()=>{

    sidebar.classList.toggle('active');

    overlay.style.display=
    sidebar.classList.contains('active')
    ?'block'
    :'none';

});

overlay?.addEventListener('click',()=>{

    sidebar.classList.remove('active');
    overlay.style.display='none';

});

// =========================
// STREAM
// =========================

document.getElementById('startStream')
?.addEventListener('click',()=>{

    cameraStream.src=
    API.stream+'?t='+Date.now();

});

document.getElementById('stopStream')
?.addEventListener('click',()=>{

    cameraStream.src='';

});

document.getElementById('snapshotBtn')
?.addEventListener('click',()=>{

    window.open(
        API.snapshot,
        '_blank'
    );

});

// =========================
// STATUS
// =========================

async function loadStatus(){

try{

    const res=
    await fetch(API.status);

    const data=
    await res.json();

    document.getElementById('cpuUsage').textContent=
    data.cpu+'%';

    document.getElementById('ramUsage').textContent=
    data.ram+' MB';

    document.getElementById('rssiValue').textContent=
    data.rssi+' dBm';

    document.getElementById('uptime').textContent=
    data.uptime;

    document.getElementById('sdUsage').textContent=
    data.sd+'%';

}
catch(err){

    console.error(err);

}

}

// =========================
// FILES
// =========================

async function loadFiles(){

try{

    const res=
    await fetch(API.files);

    const files=
    await res.json();

    const container=
    document.getElementById('fileList');

    container.innerHTML='';

    files.forEach(file=>{

        container.innerHTML+=`
        <div class="file-row">
            <i class="fa-solid fa-file"></i>
            ${file.name}
            <span>${file.size}</span>
        </div>
        `;

    });

}
catch(err){

    console.error(err);

}

}

document.getElementById('refreshFiles')
?.addEventListener(
'click',
loadFiles
);

// =========================
// RECORDING
// =========================

document.getElementById('recordStart')
?.addEventListener('click',async()=>{

    await fetch(
        API.recordStart,
        {method:'POST'}
    );

    alert('Recording Started');

});

document.getElementById('recordStop')
?.addEventListener('click',async()=>{

    await fetch(
        API.recordStop,
        {method:'POST'}
    );

    alert('Recording Stopped');

});

// =========================
// OTA
// =========================

document.getElementById('uploadFirmware')
?.addEventListener('click',()=>{

    const file=
    document.getElementById(
        'firmwareFile'
    ).files[0];

    if(!file){

        alert('Select firmware');

        return;

    }

    const fd=
    new FormData();

    fd.append(
        'firmware',
        file
    );

    const xhr=
    new XMLHttpRequest();

    xhr.upload.addEventListener(
        'progress',
        e=>{

        const percent=
        Math.round(
            (e.loaded/e.total)*100
        );

        document
        .getElementById(
            'progressBar'
        )
        .style.width=
        percent+'%';

    });

    xhr.open(
        'POST',
        API.ota
    );

    xhr.send(fd);

});

// =========================
// SETTINGS
// =========================

document.getElementById('saveSettings')
?.addEventListener(
'click',
async()=>{

const payload={

    ssid:
    document.getElementById(
        'ssid'
    ).value,

    password:
    document.getElementById(
        'wifiPass'
    ).value,

    ip:
    document.getElementById(
        'staticIP'
    ).value,

    resolution:
    document.getElementById(
        'resolution'
    ).value

};

await fetch(
API.config,
{

    method:'POST',

    headers:{
        'Content-Type':
        'application/json'
    },

    body:
    JSON.stringify(
        payload
    )

}
);

alert('Config Saved');

});

// =========================
// AI
// =========================

async function loadAI(){

try{

    const res=
    await fetch(API.ai);

    const data=
    await res.json();

    document.getElementById(
        'detectedObject'
    ).textContent=
    data.object;

    document.getElementById(
        'confidence'
    ).textContent=
    data.confidence+'%';

}
catch(err){

    console.error(err);

}

}

// =========================
// CHART
// =========================

const ctx=
document.getElementById(
'systemChart'
);

if(ctx){

new Chart(ctx,{

type:'line',

data:{

labels:[
'1','2','3','4','5'
],

datasets:[{

label:'CPU',

data:[
12,24,18,35,22
],

borderColor:'#00b4ff',

fill:false

}]

},

options:{

responsive:true

}

});

}

// =========================
// AUTO REFRESH
// =========================

setInterval(
loadStatus,
3000
);

setInterval(
loadAI,
5000
);

loadStatus();
loadFiles();
loadAI();