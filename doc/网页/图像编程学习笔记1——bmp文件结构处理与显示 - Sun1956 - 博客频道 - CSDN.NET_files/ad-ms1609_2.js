//bbs 
var cloudad_type2 = 'ms1609_2';
var cloudad_urls2 = [
'http://ad.csdn.net/adsrc/hp-apr-server-pip-300-250-30k.swf'
];
var cloudad_clks2 = [
''
];

var can_swf2 = (function () {
    if (document.all) swf = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
    else if (navigator.plugins) swf = navigator.plugins["Shockwave Flash"];
    return !!swf;
})();

function cloudad_show2() {
    var rd = Math.random();
    var ad_url, log_url;
    if (can_swf2) {
        ad_url = cloudad_urls2[0];

        log_url = 'http://ad.csdn.net/log.ashx';
        log_url += '?t=view&adtype=' + cloudad_type2 + '&adurl=' + encodeURIComponent(ad_url);
        cloudad_doRequest2(log_url, true);
    }
    if (rd < 0) {
        ad_url = cloudad_clks2[0];

        log_url = 'http://ad.csdn.net/log.ashx';
        log_url += '?t=click&adtype=' + cloudad_type2 + '&adurl=' + encodeURIComponent(ad_url);
        cloudad_doRequest2(log_url, true);
    }
}

function cloudad_doRequest2(url, useFrm) {
    var e = document.createElement(useFrm ? "iframe" : "img");

    e.style.width = "1px";
    e.style.height = "1px";
    e.style.position = "absolute";
    e.style.visibility = "hidden";

    if (url.indexOf('?') > 0) url += '&r_m=';
    else url += '?r_m=';
    url += new Date().getMilliseconds();
    e.src = url;

    document.body.appendChild(e);
}

setTimeout(function () {
    cloudad_show2();
}, 1000);
