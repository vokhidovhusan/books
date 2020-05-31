//linkFetcher.debug("fetch_article_orig_links.js");
var links = document.getElementsByTagName("a");
for (var i = 0; i < links.length; ++i) {
    if (links[i].href.search("/articles/") != -1) {
	linkFetcher.addUrlAndName(links[i].href, links[i].innerHTML);
	/*
	linkFetcher.debug(links[i].href + " * " + links[i].innerHTML);
	*/
    }
}
//linkFetcher.debug("fetch_article_orig_links.js OK");
