// New late 2009 script; with modification for changes in 2010
//linkFetcher.debug("fetch_issue_links.js");
function getRequest() {
    var factories = [function() {return new XMLHttpRequest();},
        function() {return new ActiveXObject("Microsoftt.XMLHTTP");},
        function() {return new ActiveXObject("MSXML2.XMLHTTP.3.0");},
        function() {return new ActiveXObject("MSXML2.XMLHTTP");}];
    for (var i = 0; i < factories.length; ++i) {
        try {
            var factory = factories[i];
            var request = factory();
            if (request != null)
                return request;
        }
        catch (err) {
            continue;
        }
    }
    throw new Error("XMLHttpRequest is not supported");
}

/* Pre-2010
var regex = new RegExp("href=.([/]contents[/]\\d{8}).>([^<]+)", "gim");
*/
var regex = new RegExp(
        "href=\"(/issues/\\d{4}/[a-z]+/\\d\\d?/)\">([^<]+)", "gim");
var years = 5; // Was 10 but that seems a bit much
var today = new Date();
for (var year = today.getFullYear() - years; year <= today.getFullYear();
     ++year) {
    /* Pre-2010
    var url = "http://www.nybooks.com/archives/browse?year=" + year;
    */
    var url = "http://www.nybooks.com/issues/" + year + "/";
    //linkFetcher.debug("checking " + url);
    var request = getRequest();
    request.onreadystatechange = function() {
        if (request.readyState == 4) {
            if (request.status == 200) {
                var text = request.responseText;
                var match;
                while ((match = regex.exec(text)) != null) {
                    var url = match[1];
                    var issue = match[2];
                    // Post-2010 (start)
                    if (issue.charAt(0) == "V")
                        continue;
                    // Post-2010 (end)
                    linkFetcher.addUrlAndName(url, issue);
                    //linkFetcher.debug("found " + url + " " + issue);
                }
            }
        }
    }
    request.open("GET", url, false);
    request.send(null);
}
//linkFetcher.debug("fetch_issue_links.js OK");
