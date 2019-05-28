import { Injectable } from '@angular/core';
import { User } from "../model/user.model";
declare const pskclientRequire: any;

@Injectable()
export class UserService {
    ris: any;

    constructor() {
        const interact = pskclientRequire("interact");
        interact.enableRemoteInteractions();
        this.ris = interact.createRemoteInteractionSpace('testRemote', 'http://127.0.0.1:8080', 'local/agent/example');
        this.ris.startSwarm('UserManagement', 'init', 'usermanagement').onReturn(() => {
            console.log('SWARM started');
        });
    }

    getUsers(callback) {
        this.ris.startSwarm('UserManagement', 'listUsers', 'usermanagement').onReturn(callback);
    }

    getUserById(id: string, callback) {
        this.ris.startSwarm('UserManagement', 'getUserById', 'usermanagement', id).onReturn(callback);
    }

    createUser(user: User, callback) {
        user.id = `id${Math.floor(Math.random() * 10000)}`;
        this.ris.startSwarm('UserManagement', 'addUser', 'usermanagement', user).onReturn(callback);
    }

    updateUser(user: User, callback) {
        this.ris.startSwarm('UserManagement', 'addUser', 'usermanagement', user).onReturn(callback);
    }

    deleteUser(id: string, callback){
        this.ris.startSwarm('UserManagement', 'removeUser', 'usermanagement', id).onReturn(callback);
    }
}
