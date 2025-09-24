package org.example.studentcrud.repository;

import org.example.studentcrud.model.Student;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.orm.jpa.DataJpaTest;
import org.springframework.boot.test.autoconfigure.orm.jpa.TestEntityManager;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@DataJpaTest
public class StudentRepositoryTest {
    
    @Autowired
    private TestEntityManager entityManager;
    
    @Autowired
    private StudentRepository studentRepository;
    
    @Test
    public void testFindByName() {
        // Подготовка данных
        Student student1 = new Student("John Doe", "CS-101");
        Student student2 = new Student("Jane Smith", "MATH-201");
        Student student3 = new Student("John Doe", "PHYS-301");
        
        entityManager.persist(student1);
        entityManager.persist(student2);
        entityManager.persist(student3);
        entityManager.flush();
        
        // Выполнение теста
        List<Student> students = studentRepository.findByName("John Doe");
        
        // Проверка результата
        assertEquals(2, students.size());
        assertEquals("John Doe", students.get(0).getName());
        assertEquals("John Doe", students.get(1).getName());
    }
    
    @Test
    public void testSaveStudent() {
        Student student = new Student("Test Name", "Test Group");
        
        Student saved = studentRepository.save(student);
        
        assertNotNull(saved.getId());
        assertEquals("Test Name", saved.getName());
        assertEquals("Test Group", saved.getGroup());
    }
}