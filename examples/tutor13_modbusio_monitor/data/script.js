/*=============== SCROLL SECTIONS ACTIVE LINK ===============*/
const sections = document.querySelectorAll('section[id]')

function scrollActive(){
    const scrollY = window.pageYOffset

    sections.forEach(current =>{
        const sectionHeight = current.offsetHeight,
            sectionTop = current.offsetTop - 50,
            sectionId = current.getAttribute('id')

        if(scrollY > sectionTop && scrollY <= sectionTop + sectionHeight){
            document.querySelector('.nav__menu a[href*=' + sectionId + ']').classList.add('active-link')
        }else{
            document.querySelector('.nav__menu a[href*=' + sectionId + ']').classList.remove('active-link')
        }
    })
}
window.addEventListener('scroll', scrollActive)


/*=============== CHANGE BACKGROUND HEADER ===============*/
function scrollHeader(){
    const header = document.getElementById('header')
    // When the scroll is greater than 80 viewport height, add the scroll-header class to the header tag
    if(this.scrollY >= 80) header.classList.add('scroll-header'); else header.classList.remove('scroll-header')
}
window.addEventListener('scroll', scrollHeader)

/*----------------------------------------------------------

----------------------------------------------------------*/
// Get current sensor readings when the page loads
window.addEventListener('load', getReadings);

let DiagMsg = [];

//Plot temperature in the temperature chart
function updateValue(jsonValue) {
  var keys = Object.keys(jsonValue);
  //clearDiagnostic();    //- Clear Diagnostic table before update
  for (var i = 0; i < keys.length; i++){
    var x = (new Date()).getTime();
    //console.log(x);
    const key = keys[i];
    var keyName = keys[i];
    if(i < 208){
      var y = Number(jsonValue[key]);
    }else{
      var y = jsonValue[key];
    }
    
    const elem = document.querySelector('#m'+i+'_n');
    if(i<64){  
      if(elem !== document.activeElement){ 
        document.getElementById('m'+ i +'_n').innerHTML = keyName;
      }  
      document.getElementById('m'+ i +'_v').innerHTML = y;  
      document.getElementById('m'+ i +'_u').innerHTML = new Date(x).toLocaleTimeString("en-US");
    }else if((i>=64)&&(i<128)){
      document.getElementById('d'+ (i-64) +'_n').innerHTML = keyName;
      document.getElementById('d'+ (i-64) +'_v').innerHTML = y;
      document.getElementById('d'+ (i-64) +'_u').innerHTML = new Date(x).toLocaleTimeString("en-US");
    }else if((i>=128)&&(i<192)){
      document.getElementById('f'+ (i-128) +'_n').innerHTML = keyName;
      document.getElementById('f'+ (i-128) +'_v').innerHTML = y.toFixed(2);
      document.getElementById('f'+ (i-128) +'_u').innerHTML = new Date(x).toLocaleTimeString("en-US");
    }else if((i>=192)&&(i<200)){
      if(y==1){
        document.getElementById('i'+ (i-192)+'_lamp').style.backgroundColor = "#00FF00";
      }else{
        document.getElementById('i'+ (i-192)+'_lamp').style.backgroundColor = "aliceblue";
      }
    }else if((i>=200)&&(i<208)){
      if(y==1){
        document.getElementById('o'+ (i-200)+'_lamp').style.backgroundColor = "#FF5733";
      }else{
        document.getElementById('o'+ (i-200)+'_lamp').style.backgroundColor = "aliceblue";
      }
    }else if(i >= 208){     //- Diagnostic Msg
      var count = i - 207;
      if(count > DiagMsg.length){
        insertDiagnostic(keyName,y);
        DiagMsg[count - 1] = y;
        console.log(DiagMsg[count - 1]);    //- Show last DiagMsg
      }
    }  
  }
}

// Function to get current readings on the webpage when it loads for the first time
function getReadings(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        //console.log(myObj);
        updateValue(myObj);
      }
    };
    xhr.open("GET", "/readings", true);
    xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('message', function(e) {
    console.log("message", e.data);
  }, false);

  source.addEventListener('new_readings', function(e) {
    //console.log("new_readings", e.data);
    var myObj = JSON.parse(e.data);
    //console.log(myObj);
    updateValue(myObj);
  }, false);
}

function editData(event){
  //event.preventDefault();
  var ex = event;
  var keyCode = ex.keyCode || ex.which;
  if(keyCode == 13){
    ex.preventDefault();
    document.getElementById(ex.target.id).blur();
  }
  console.log(ex.target.id);
}

function insertDiagnostic(tagName='None',val='None'){
  var x = (new Date()).getTime();
  var table = document.getElementById("diagnostable");
  var totalRowCount = table.rows.length;
  if(totalRowCount > 50){
    clearDiagnostic();
    totalRowCount = 0;
  }
  var row = table.insertRow(1);
  var cell0 = row.insertCell(0);
  var cell1 = row.insertCell(1);
  var cell2 = row.insertCell(2);
  var cell3 = row.insertCell(3);
  cell0.innerHTML = totalRowCount - 1;
  cell1.innerHTML = tagName;
  cell2.innerHTML = val;
  cell3.innerHTML = new Date(x).toLocaleTimeString("en-US");;
}

function clearDiagnostic(){
  var elmtTable = document.getElementById('diagnostable');
  var tableRows = elmtTable.getElementsByTagName('tr');
  var rowCount = tableRows.length;
  if(rowCount > 0){
    while(elmtTable.rows.length > 2){
      elmtTable.deleteRow(1);
    }
  }else{
    alert('No rows data')
  }
  
}