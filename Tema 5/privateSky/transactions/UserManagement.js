$$.transaction.describe("UserManagement", {
    init: function (alias) {
        let transaction = $$.blockchain.beginTransaction({});
        let userManager = transaction.lookup('global.UserManager', alias);

        userManager.init(alias);

        try {
            transaction.add(userManager);
            $$.blockchain.commit(transaction);
        } catch (err) {
            this.return("UserManager creating failed!");
            return;
        }

        this.return(null, alias);
    },

    addUser: function (alias, user) {
        let transaction = $$.blockchain.beginTransaction({});
        let userManager = transaction.lookup('global.UserManager', alias);

        let result = userManager.addUser(user.id, user);

        try {
            transaction.add(userManager);
            $$.blockchain.commit(transaction);
        } catch (err) {
            this.return("Failed to save UserManager update!");
            return;
        }

        this.return(null, result);
    },

    removeUser: function (alias, userId) {
        let transaction = $$.blockchain.beginTransaction({});
        let userManager = transaction.lookup('global.UserManager', alias);

        let result = userManager.removeUser(userId);

        try {
            transaction.add(userManager);
            $$.blockchain.commit(transaction);
        } catch (err) {
            this.return("Failed to save UserManager update!");
            return;
        }

        this.return(null, result);
    },

    listUsers: function (alias) {
        let transaction = $$.blockchain.beginTransaction({});
        let userManager = transaction.lookup('global.UserManager', alias);

        let result = userManager.listUsers();

        this.return(null, result);
    },

    getUserById: function (alias, userId) {
        let transaction = $$.blockchain.beginTransaction({});
        let userManager = transaction.lookup('global.UserManager', alias);

        let result = userManager.getUserById(userId);

        this.return(null, result);
    }
});
