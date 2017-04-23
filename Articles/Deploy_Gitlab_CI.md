Deploy Gitlab CI
================

[linux-repository](https://docs.gitlab.com/runner/install/linux-repository.html)
[using-docker-build](https://docs.gitlab.com/ce/ci/docker/using_docker_build.html)


1. install Gitlab runner
2. use shell executor
3. add ssh key to gitlab server
4. if the gitlab runner doesn't locate on the same host with gitlab server, maybe you need to proxy the request(e.g. using a docker nginx proxy)
5. write .gitlab-ci.yml
6. build


