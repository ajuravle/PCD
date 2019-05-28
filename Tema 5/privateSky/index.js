if(typeof $$ !== "undefined" && typeof $$.blockchain === "undefined"){
    const pskDB = require("pskdb");
    const pds = pskDB.startDB("./db");
}

module.exports = $$.library(function(){
	require("./assets/UserManager.js");
    require("./transactions/UserManagement.js");
});