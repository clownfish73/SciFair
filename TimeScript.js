setInterval(Action,1)
setInterval(ChangeColor,1)
setInterval(ChangeBG,10)
let color=0;
let colorBG=0;
function FormatTime(time){
  return (((time>9) ? "" : "0")+time)
}
function Action(){
  let time = new Date();
  
  let parsed = "Time right now: "+FormatTime(time.getHours())+":"+FormatTime(time.getMinutes())+":"+FormatTime(time.getSeconds());
  
  document.getElementById("Ms").innerHTML = "Milliseconds elapsed: "+time.getMilliseconds();
  
  document.getElementById("Main").innerHTML = parsed;
  
  document.getElementById("Date").innerHTML = "Date: "+time.getDate()+"/"+(time.getMonth()+1)+"/"+time.getFullYear();
  
}

function ChangeColor(){
  for (let x of document.getElementsByTagName("h2")){
    x.style.color=`hsl(${color}, 100%, 70%)`;
  }
  document.getElementById("Top").style.color = `hsl(${color}, 100%, 70%)`;
  color = (color+10);
}

function ChangeBG(){
  for (let i of document.getElementsByTagName("html")){
    i.style.backgroundColor=`hsl(${colorBG}, 100%, 50%)`;
  }
  colorBG = (colorBG+1000);
}