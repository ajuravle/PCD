import { RouterModule, Routes } from '@angular/router';
import { AddUserComponent } from "./add-user/add-user.component";
import { ListUsersComponent } from "./list-user/list-user.component";
import { EditUserComponent } from "./edit-user/edit-user.component";

const routes: Routes = [
  { path: 'add-user', component: AddUserComponent },
  { path: 'list-users', component: ListUsersComponent },
  { path: 'edit-user/:id', component: EditUserComponent },
  {
    path: '**',
    redirectTo: '/list-users',
    pathMatch: 'full'
  },
];

export const routing = RouterModule.forRoot(routes);
