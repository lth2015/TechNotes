SELECT user.name, organization.name FROM user RIGHT JOIN organization ON user.orgId=organization.id;
SELECT user.name, organization.name FROM user RIGHT JOIN organization ON user.orgId=organization.id WHERE organiztion.name="dev";
