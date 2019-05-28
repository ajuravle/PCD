import { Component, OnInit } from '@angular/core';
import { UserService } from "../service/user.service";
import { Router, ActivatedRoute } from "@angular/router";
import { User } from "../model/user.model";
import { FormBuilder, FormGroup, Validators } from "@angular/forms";
import { first } from "rxjs/operators";

@Component({
  selector: 'app-edit-user',
  templateUrl: './edit-user.component.html',
  styleUrls: ['./edit-user.component.css']
})
export class EditUserComponent implements OnInit {

  user: User;
  editForm: FormGroup;
  constructor(
    private formBuilder: FormBuilder,
    private router: Router,
    private userService: UserService,
    private route: ActivatedRoute) { }

  ngOnInit() {
    this.editForm = this.formBuilder.group({
      id: [],
      email: ['', Validators.required],
      firstName: ['', Validators.required],
      lastName: ['', Validators.required]
    });


    let userId = this.route.snapshot.paramMap.get("id")
    this.userService.getUserById(userId, (error, data) => {
        this.editForm.setValue(data);
      });
  }

  onSubmit() {
    this.userService.updateUser(this.editForm.value, (error, data) => {
          this.router.navigate(['list-user']);
        });
  }

}
