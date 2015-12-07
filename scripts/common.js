var keys_height = 0;
var mobile = true;

function adjustScreen(){
    var height = $(window).height();
    $("section#intro").css({height: height});
    $("section#footer").css({height: height/3});
    keys_height = parseInt(($(window).height())/2);
    $("section.content:not(:first)").css({height: keys_height*2});
    if(mobile){
        $("section#home").css({height: keys_height*4+"px"});
        $("section#home").find("div.cell-center").css({marginTop: 2*keys_height+"px"});
    } else {
        $("section#home").css({height: keys_height*3+"px"});
        $("section#home").find("div.cell-center").css({marginTop: keys_height+"px"});
    }
}

$(document).ready(function(){

    if( !(navigator.platform.indexOf("iPhone") != -1) &&
        !(navigator.platform.indexOf("iPad") != -1) ) mobile = false;
    
    $(document).on("click", "div#more", function(){
        $('html, body').animate({scrollTop: $("section#intro").height()});
    }); 

    adjustScreen(); $(window).resize(adjustScreen);
});

