import { Component, OnInit } from '@angular/core';
import { Router } from "@angular/router";
import { UserService } from "../service/user.service";
import { User } from "../model/user.model";

@Component({
  selector: 'app-list-user',
  templateUrl: './list-user.component.html',
  styleUrls: ['./list-user.component.css']
})
export class ListUsersComponent implements OnInit {

  users: User[];

  constructor(private router: Router, private userService: UserService) { }

  ngOnInit() {
    this.userService.getUsers((error, data) => {
      this.users = data;
    });
  }

  addUser(): void {
    this.router.navigate(['add-user']);
  };

  editUser(user: User): void {
    this.router.navigate(['edit-user', user.id]);
  };

  deleteUser(user: User): void {
    this.userService.deleteUser(user.id, (error, data) => {
      this.users = this.users.filter(u => u !== user);
    });
  };
}
