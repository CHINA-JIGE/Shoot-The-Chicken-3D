COMSCORE.SiteRecruit.Broker.config={version:"5.1.3",cddsDomains:"www.windowsphone.com|www.microsoftstore.com|office.com|xbox.com",cddsInProgress:"cddsinprogress",domainSwitch:"tracking3p",domainMatch:"([\\da-z.-]+.com)",delay:0,cddsIntervalMax:10,crossDomainCheck:function(){if(this.cddsIntervalMax>1){this.cddsIntervalMax--;
if(COMSCORE.SiteRecruit.Utils.UserPersistence.getCookieValue(this.cddsInProgress)!=false){setInterval(function() { COMSCORE.SiteRecruit.DDKeepAlive.setDDTrackerCookie()}, 1000);
COMSCORE.SiteRecruit._halt=true;this.clearCrossDomainCheck();}}else{this.clearCrossDomainCheck();
}},clearCrossDomainCheck:function(){window.clearInterval(crossDomainInterval);},isolateDomain:function(A){A=A.substring(A.indexOf("//")+2,A.length);
A=A.substring(0,A.indexOf("/"));return A;},testMode:false,addEventDelay:1000,cookie:{name:"msresearch",path:"/",domain:".microsoft.com",duration:90,rapidDuration:0,expireDate:""},tracker:{std:"http://www.microsoft.com/library/svy/sto/https/SiteRecruit_Tracker.htm",ssl:"https://www.microsoft.com/library/svy/sto/https/SiteRecruit_Tracker.htm"},mobile:{match:"iphone|ipad|ipod|android|opera mini|blackberry|windows (phone|ce)|iemobile|htc|nokia|bb10|mobile safari|mobile",kmatch:"(?:; ([^;)]+) Build/.*)?\bSilk/([0-9._-]+)\b(.*\bMobile Safari\b)?",halt:true},graceIncr:{name:"graceIncr",initDelay:0,clickDelay:5000,match:"https://(account|accounts|billing|commerce|support|login|live).(microsoft|live|xbox).(com)",altTag:"class",htmlMatch:"sign in"},prefixUrl:"",mapping:[{m:"msdn.microsoft.com/en-us",c:"inv_c_p246609455.js",f:0.0143,p:0},{m:"technet.microsoft.com/en-us",c:"inv_c_p246609455-technet.js",f:0.033,p:0}],Events:{beforeRecruit:function(){var C=COMSCORE.SiteRecruit.Broker.config;
if(/windowsmediaplayer|(microsoft|microsoftbusinesshub|xbox|windowsphone|windows|skype|login.live|microsoftvirtualacademy|microsoftvolumelicensing|microsoftstore|office|onenote|visualstudio)\.com/i.test(document.referrer)){
	COMSCORE.SiteRecruit._halt = true;
}
var B=COMSCORE.SiteRecruit.Utils.UserPersistence;COMSCORE.SiteRecruit.Broker.custom={captLinks:function(F){var E=B.getCookieValue("captLinks");
var D="";if(E==false){D=escape(F)+";";}else{if(D.length+E.length<1440){D=E+escape(F)+";";
}}if(D!=""){B.createCookie("captLinks",D,{path:"/",domain:C.cookie.domain,duration:"s"});
}},allTags:function(H,J,I,G){if(H=="class"){if(/msie (8|7)/i.test(navigator.userAgent)){return;
}var D=document.getElementsByClassName(J);}else{var D=document.getElementsByTagName(H);
}var E=new RegExp(I,"i");for(var F=0;F<D.length;F++){if((H=="a"&&E.test(D[F].href))||(H=="class"&&E.test(D[F].innerHTML))){if(D[F].addEventListener){if(G==1){D[F].addEventListener("click",function(K){if(E.test(this.href)){B.createCookie(C.domainSwitch,C.isolateDomain(this.href),{path:"/",domain:C.cookie.domain,duration:"s"});
}},false);}else{if(G==2){D[F].addEventListener("click",function(K){B.createCookie("graceIncr",1,{path:"/",domain:C.cookie.domain,duration:"s"});
},false);}else{if(G==3&&COMSCORE.isDDInProgress()){D[F].addEventListener("click",function(K){COMSCORE.SiteRecruit.Broker.custom.captLinks(this.href);
},false);}}}}else{hrefURL=D[F].href;if(G==1){D[F].attachEvent("onclick",function(){B.createCookie(C.domainSwitch,C.isolateDomain(hrefURL),{path:"/",domain:C.cookie.domain,duration:"s"});
});}else{if(G==2){D[F].attachEvent("onclick",function(){B.createCookie("graceIncr",1,{path:"/",domain:C.cookie.domain,duration:"s"});
});}else{if(G==3&&COMSCORE.isDDInProgress()){D[F].attachEvent("onclick",function(){COMSCORE.SiteRecruit.Broker.custom.captLinks(this.href);
});}}}}}}}};var A=0;if(COMSCORE.SiteRecruit.Utils.UserPersistence.getCookieValue("graceIncr")==1){A=5000;
}setTimeout(function(){COMSCORE.SiteRecruit.Utils.UserPersistence.createCookie("graceIncr",0,{path:"/",domain:C.cookie.domain,duration:"s"});
},A);setTimeout(function(){COMSCORE.SiteRecruit.Broker.custom.allTags("a","",C.cddsDomains,1);
},C.addEventDelay);setTimeout(function(){COMSCORE.SiteRecruit.Broker.custom.allTags("a","",C.graceIncr.match,2);
},C.addEventDelay);}}};var crossDomainInterval=window.setInterval("COMSCORE.SiteRecruit.Broker.config.crossDomainCheck()","1000");
if(COMSCORE.SiteRecruit.Broker.delayConfig==true){COMSCORE.SiteRecruit.Broker.config.delay=5000;
}window.setTimeout("COMSCORE.SiteRecruit.Broker.run()",COMSCORE.SiteRecruit.Broker.config.delay);