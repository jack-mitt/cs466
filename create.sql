CREATE TABLE Member
(
 firstName VARCHAR(255) NOT NULL,
 lastName VARCHAR(255) NOT NULL,
 emailAddress VARCHAR(255) NOT NULL,
 isPayingMember CHAR(1) CHECK(isPayingMember IN ('y','n')) NOT NULL,
 isStaffMember CHAR(1) CHECK(isStaffMember IN ('y','n')) NOT NULL,
 isProblemContributor CHAR(1) CHECK(isProblemContributor IN ('y','n')) NOT NULL,
 subscriptionStartDate DATE,
 PRIMARY KEY (emailAddress)
);

CREATE TABLE Problem
(
 title VARCHAR(255) NOT NULL,
 difficulty VARCHAR(10) NOT NULL,
 type VARCHAR(20) CHECK(type IN ('algorithm','database','dataStructure')) NOT NULL,
 ContributorEmail VARCHAR(255) NOT NULL,
 StaffEmail VARCHAR(255),
 numberOfRevisions INTEGER NOT NULL,
 PRIMARY KEY (title),
 FOREIGN KEY (ContributorEmail ) REFERENCES Member (emailAddress),
 FOREIGN KEY (StaffEmail ) REFERENCES Member (emailAddress)
);

CREATE TABLE BuildUpon
(
 thisProblem VARCHAR(255) NOT NULL,
 otherProblem VARCHAR(255) NOT NULL,
 PRIMARY KEY (thisProblem, otherProblem),
 FOREIGN KEY (thisProblem ) REFERENCES Problem (title),
 FOREIGN KEY (otherProblem ) REFERENCES Problem (title)
);

CREATE TABLE RelatedTopic
(
 title VARCHAR(255) NOT NULL,
 topicName VARCHAR(255) NOT NULL,
 PRIMARY KEY (title, topicName),
 FOREIGN KEY (title) REFERENCES Problem (title)
);

CREATE TABLE ProblemsPool
(
 emailAddress VARCHAR(255) NOT NULL,
 poolName VARCHAR(255) NOT NULL,
 PRIMARY KEY (emailAddress, poolName),
 FOREIGN KEY (emailAddress) REFERENCES Member (emailAddress)
);

CREATE TABLE ComposedOf
(
 title VARCHAR(255) NOT NULL,
 emailAddress VARCHAR(255) NOT NULL,
 poolName VARCHAR(255) NOT NULL,
 PRIMARY KEY (title, emailAddress, poolName),
 FOREIGN KEY (title) REFERENCES Problem (title),
 FOREIGN KEY (emailAddress, poolName) REFERENCES ProblemsPool (emailAddress, poolName)
);
