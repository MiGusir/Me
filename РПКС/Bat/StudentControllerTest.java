package org.example.studentcrud.controller;

import org.example.studentcrud.model.Student;
import org.example.studentcrud.service.StudentService;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@WebMvcTest(StudentController.class)
public class StudentControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private StudentService studentService;
    
    @Test
    public void testCreateStudent() throws Exception {
        Student student = new Student("John Doe", "CS-101");
        Mockito.when(studentService.createStudent(any(Student.class))).thenReturn(student);
        
        mockMvc.perform(post("/api/students")
                .contentType(MediaType.APPLICATION_JSON)
                .content("{\"name\":\"John Doe\",\"group\":\"CS-101\"}"))
                .andExpect(status().isCreated())
                .andExpect(jsonPath("$.name").value("John Doe"))
                .andExpect(jsonPath("$.group").value("CS-101"));
    }
    
    @Test
    public void testGetAllStudents() throws Exception {
        List<Student> students = Arrays.asList(
            new Student("John Doe", "CS-101"),
            new Student("Jane Smith", "MATH-201")
        );
        Mockito.when(studentService.getAllStudents()).thenReturn(students);
        
        mockMvc.perform(get("/api/students"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$[0].name").value("John Doe"))
                .andExpect(jsonPath("$[1].name").value("Jane Smith"));
    }
    
    @Test
    public void testGetStudentById() throws Exception {
        Student student = new Student("John Doe", "CS-101");
        Mockito.when(studentService.getStudentById(1L)).thenReturn(Optional.of(student));
        
        mockMvc.perform(get("/api/students/1"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.name").value("John Doe"));
    }
    
    @Test
    public void testUpdateStudent() throws Exception {
        Student student = new Student("Updated Name", "Updated Group");
        Mockito.when(studentService.updateStudent(eq(1L), any(Student.class))).thenReturn(student);
        
        mockMvc.perform(put("/api/students/1")
                .contentType(MediaType.APPLICATION_JSON)
                .content("{\"name\":\"Updated Name\",\"group\":\"Updated Group\"}"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.name").value("Updated Name"));
    }
}