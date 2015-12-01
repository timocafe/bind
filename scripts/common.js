var keys_height = 0;
var active_part = "part0";
var locator = 0;
var locator_max = 1;
var mobile = true;

function adjustScreen(){
    var height = $(window).height();
    $("section#intro").css({height: height});
    $("div#cover").css({height: height});
    keys_height = parseInt(($(window).height())/2);
    $("section.content:not(:first)").css({height: keys_height*2});
    if(mobile){
        $("section#home").css({height: keys_height*4+"px"});
        $("section#home").find("div.cell-center").css({marginTop: 2*keys_height+"px"});
    } else {
        $("section#home").css({height: keys_height*3+"px"});
        $("section#home").find("div.cell-center").css({marginTop: keys_height+"px"});
    }
    $("div#example-view-container").css({height: keys_height*2-1});
    $("div#example-view").css({height: keys_height*2-1});

    if($("div#example-view").find("pre").length != 0){
        var width = ($(window).width()-900)/2+180;
        $("div#example-view-container").css({width: width});
        $("div#example-view").find("pre").css({height: Math.min(parseInt(content.css("height")), 2*keys_height-60)});
    }
}

$(document).ready(function(){

    if( !(navigator.platform.indexOf("iPhone") != -1) &&
        !(navigator.platform.indexOf("iPad") != -1) ) mobile = false;
    
    $("div.example-link").click(function(){
        var content = $("div.example-content#"+$(this).data("target"));
        $("div#example-view").html(content.html());
        $("div.example-link").css({color: "#41B0DB"});
        $(this).css({color: "white"});
        adjustScreen();
    });

    $(document).on("click", "div#more", function(){
        $('html, body').animate({scrollTop: $("section#intro").height()});
    }); 

    $("img.sponsor").mouseenter(function(){ $(this).css("-webkit-filter", "grayscale(.0)");  });
    $("img.sponsor").mouseleave(function(){ $(this).css("-webkit-filter", "grayscale(.9)");  });

    adjustScreen(); $(window).resize(adjustScreen);

    $("img#next").click(function(){
        var current = locator; if(++locator == locator_max) locator = 0;
        var next = locator;

        $("div.how").animate({left: "-=100px", opacity: 0}, 200, function(){ 
            $(this).css({display: "none", opacity: 1, left: "+=100px"}); 
        });
        setTimeout(function(){
            $("div.how#part"+locator).css({display: "block", opacity: 0}).animate({opacity: 1}, 400);
        }, 300);

        $("div#cover"+current).animate({left: "-"+$(window).width()});
        $("div#cover"+next).css({left: $(window).width()}).animate({left: 0});
        $("img#prev").css("opacity", 0.5);
    });
    $("img#prev").click(function(){
        var current = locator; if(--locator == -1) locator = locator_max-1;
        var next = locator;

        $("div.how").animate({left: "+=100px", opacity: 0}, 200, function(){ 
            $(this).css({display: "none", opacity: 1, left: "-=100px"}); 
        });
        setTimeout(function(){
            $("div.how#part"+locator).css({display: "block", opacity: 0}).animate({opacity: 1}, 400);
        }, 300);

        $("div#cover"+current).animate({left: $(window).width()});
        $("div#cover"+next).css({left: -$(window).width()}).animate({left: 0});
    });

    $("div#github").click(function(){
        window.open('https://github.com/akosenkov/ambient');
    });
    $("div#docs").click(function(){
        window.open('guide.pdf');
    });
    $("img.revert").click(function(){
        window.location.replace('http://ambientcxx.org');
    });
    $("div#make-options > div").click(function(){
        var target = $(this).attr("id");
        $("pre.make").css({display: "none"});
        $("pre.make#"+target).css({display: "block"});
        $("div#make-options > div").css({color: "#DDD"});
        $(this).css({color: "#666"});
    });
    $("div.perf").click(function(e){
        var detail = $(this).next("img.perf_detail");
        detail.css({display: "block"});
        e.stopPropagation();
        $("body").one("click", function(){
            detail.css({display: "none"});
        });
    });
    $("div.another").click(function(e){
        var container = $(this).parent();
        container.addClass("hidden");
        var next = container.next();
        if(!next.length) next = $("div.example:first");
        next.removeClass("hidden");
    });
});

