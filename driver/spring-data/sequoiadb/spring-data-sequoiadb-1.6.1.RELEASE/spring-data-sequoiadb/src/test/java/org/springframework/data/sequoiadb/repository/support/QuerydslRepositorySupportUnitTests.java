/*
 * Copyright 2011 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.springframework.data.sequoiadb.repository.support;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.sequoiadb.core.SequoiadbOperations;
import org.springframework.data.sequoiadb.core.query.Query;
import org.springframework.data.sequoiadb.repository.Person;
//import org.springframework.data.sequoiadb.repository.QPerson;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

//import com.mysema.query.sequoiadb.SequoiadbQuery;

/**
 * Unit tests for {@link QuerydslRepositorySupport}.
 * 

 */
@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration("classpath:infrastructure.xml")
public class QuerydslRepositorySupportUnitTests {

	@Autowired
    SequoiadbOperations operations;
	Person person;

	@Before
	public void setUp() {
		operations.remove(new Query(), Person.class);
		person = new Person("Dave", "Matthews");
		operations.save(person);
	}

	@Test
	public void providesSequoiadbQuery() {
//		QPerson p = QPerson.person;
//		QuerydslRepositorySupport support = new QuerydslRepositorySupport(operations) {};
//		SequoiadbQuery<Person> query = support.from(p).where(p.lastname.eq("Matthews"));
//		assertThat(query.uniqueResult(), is(person));
	}

	/**
	 * @see DATA_JIRA-1063
	 */
	@Test
	public void shouldAllowAny() {
//
//		person.setSkills(Arrays.asList("vocalist", "songwriter", "guitarist"));
//
//		operations.save(person);
//
//		QPerson p = QPerson.person;
//		QuerydslRepositorySupport support = new QuerydslRepositorySupport(operations) {};
//
//		SequoiadbQuery<Person> query = support.from(p).where(p.skills.any().in("guitarist"));
//
//		assertThat(query.uniqueResult(), is(person));
	}
}
