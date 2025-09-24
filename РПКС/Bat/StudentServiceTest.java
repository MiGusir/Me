package org.example.studentcrud.service;

import org.example.studentcrud.model.Student;
import org.example.studentcrud.repository.StudentRepository;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
public class StudentServiceTest {
    
    @Mock
    private StudentRepository studentRepository;
    
    @InjectMocks
    private StudentServiceImpl studentService;
    
    @Test
    public void testGetAllStudents() {
        List<Student> students = Arrays.asList(
            new Student("John", "CS-101"),
            new Student("Jane", "MATH-201")
        );
        when(studentRepository.findAll()).thenReturn(students);
        
        List<Student> result = studentService.getAllStudents();
        
        assertEquals(2, result.size());
        verify(studentRepository, times(1)).findAll();
    }
    
    @Test
    public void testGetStudentById() {
        Student student = new Student("John", "CS-101");
        when(studentRepository.findById(1L)).thenReturn(Optional.of(student));
        
        Optional<Student> result = studentService.getStudentById(1L);
        
        assertTrue(result.isPresent());
        assertEquals("John", result.get().getName());
        verify(studentRepository, times(1)).findById(1L);
    }
    
    @Test
    public void testCreateStudent() {
        Student input = new Student("John", "CS-101");
        Student expected = new Student("John", "CS-101");
        expected.setId(1L);
        when(studentRepository.save(input)).thenReturn(expected);
        
        Student result = studentService.createStudent(input);
        
        assertEquals("John", result.getName());
        verify(studentRepository, times(1)).save(input);
    }
    
    @Test
    public void testUpdateStudent() {
        Student existing = new Student("Old Name", "Old Group");
        Student update = new Student("New Name", "New Group");
        when(studentRepository.existsById(1L)).thenReturn(true);
        when(studentRepository.save(any(Student.class))).thenReturn(update);
        
        Student result = studentService.updateStudent(1L, update);
        
        assertNotNull(result);
        assertEquals("New Name", result.getName());
        verify(studentRepository, times(1)).existsById(1L);
        verify(studentRepository, times(1)).save(any(Student.class));
    }
}