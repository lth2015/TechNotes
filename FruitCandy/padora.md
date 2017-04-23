./index.html: ui-view class "main"
|
./views/main/login.html: ng-click(): "login()"
|
./scripts/pages/main/controller.js: $scope.jump -> $state.go('main.appManage')
|
./scripts/pages/appManage/router.js: views/appManage/appManage.html 

./views/pages/main/main.js: L97 - L110

./scripts/pages/main/main.js: L51: $scope.navList = nav.list




./scripts/pages/walkthrogh/controller.js: $state.go('main.topology')


----------------------------------------------------------------------


./index.html ui-view class "main"
./scripts/pages/main/controller.js: L65: $state.go('login')
./views/main/topology.html


------------------------------------------------------------------------------------

delete appManage.....
common/service.js
common/controller.js

--------------------------------------------------

Add namespace choose list
both backend

mv html in index.html back to topology.html, accomplishing ng-bind

---------------------------------------------------------------------

padora:

1. get all namespace list from k8s api then return back to frontend (default is default)
2. frontend request with selected namespace
3. return topology relations to frontend 

