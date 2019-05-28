$$.asset.describe("UserManager", {
    public: {
        alias: "string:alias",
        users: "map",
    },

    init: function (alias) {
        this.alias = alias;

        if (!this.users) {
            this.users = {};
        }

        return true;
    },

    addUser: function (id, user) {
        if (!this.users) {
            return false;
        }

        this.users[id] = user;

        return true;
    },

    removeUser: function (id) {
        if (!this.users) {
            return false;
        }

        this.users[id] = undefined;
        delete this.users[id];

        return true;
    },

    listUsers: function () {
        if (!this.users) {
            return false;
        }

        return Object.values(this.users);
    },

    getUserById: function (id) {
        if (!this.users) {
            return false;
        }

        return this.users[id];
    }
});