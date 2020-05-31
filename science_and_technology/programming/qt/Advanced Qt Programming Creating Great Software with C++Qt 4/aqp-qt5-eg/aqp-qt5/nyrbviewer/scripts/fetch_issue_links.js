// Original early 2009 script
//linkFetcher.debug("fetch_issue_links.js");
var forms = document.forms;
for (var i = 0; i < forms.length; ++i) {
    if (forms[i].name == "contents") {
        contentsForm = forms[i];
        break;
    }
}
for (var i = 0; i < contentsForm.elements.length; ++i) {
    if (contentsForm.elements[i].name == "issue_date") {
        comboBox = contentsForm.elements[i]
        break;
    }
}
for (var i = 0; i < comboBox.options.length; ++i) {
    var option = comboBox.options[i];
    //linkFetcher.debug(option.text + " = " + option.value);
    if (option.value != "/") {
        linkFetcher.addUrlAndName(option.value, option.text);
    }
}
//linkFetcher.debug("fetch_issue_links.js OK");
