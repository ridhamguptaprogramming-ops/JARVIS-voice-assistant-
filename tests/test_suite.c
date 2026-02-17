#include "command_processor.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>

static int file_contains(const char* path, const char* needle) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char line[4096];
    int found = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, needle) != NULL) {
            found = 1;
            break;
        }
    }

    fclose(file);
    return found;
}

static int is_directory_path(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

static int test_to_lowercase(void) {
    char* lower = to_lowercase("HeLLo C WoRLD");
    if (!lower) {
        fprintf(stderr, "to_lowercase returned NULL\n");
        return 0;
    }

    int ok = strcmp(lower, "hello c world") == 0;
    if (!ok) {
        fprintf(stderr, "Unexpected lowercase output: %s\n", lower);
    }

    free(lower);
    return ok;
}

static int test_command_contains(void) {
    if (!command_contains("build project", "build")) {
        fprintf(stderr, "Expected keyword match for 'build'\n");
        return 0;
    }

    if (command_contains("build project", "deploy")) {
        fprintf(stderr, "Unexpected keyword match for 'deploy'\n");
        return 0;
    }

    return 1;
}

static int test_extract_search_query(void) {
    const char* input = "search for c linked list";
    const char* query = extract_search_query(input);

    if (!query) {
        fprintf(stderr, "extract_search_query returned NULL\n");
        return 0;
    }

    if (strcmp(query, "c linked list") != 0) {
        fprintf(stderr, "Unexpected query extraction: %s\n", query);
        return 0;
    }

    return 1;
}

static int test_process_help(void) {
    char* response = process_command("help");
    if (!response) {
        fprintf(stderr, "process_command(help) returned NULL\n");
        return 0;
    }

    int ok = strstr(response, "daily C development tasks") != NULL;
    if (!ok) {
        fprintf(stderr, "Unexpected help response: %s\n", response);
    }

    free(response);
    return ok;
}

static int test_create_module_updates_makefile(void) {
    char original_cwd[PATH_MAX];
    if (!getcwd(original_cwd, sizeof(original_cwd))) {
        perror("getcwd");
        return 0;
    }

    char template[] = "/tmp/jarvis_module_test_XXXXXX";
    char* temp_dir = mkdtemp(template);
    if (!temp_dir) {
        perror("mkdtemp");
        return 0;
    }

    int ok = 1;

    char include_dir[PATH_MAX];
    char src_dir[PATH_MAX];
    char makefile_path[PATH_MAX];
    char header_path[PATH_MAX];
    char source_path[PATH_MAX];

    snprintf(include_dir, sizeof(include_dir), "%s/include", temp_dir);
    snprintf(src_dir, sizeof(src_dir), "%s/src", temp_dir);
    snprintf(makefile_path, sizeof(makefile_path), "%s/Makefile", temp_dir);
    snprintf(header_path, sizeof(header_path), "%s/include/auto_mod_test.h", temp_dir);
    snprintf(source_path, sizeof(source_path), "%s/src/auto_mod_test.c", temp_dir);

    if (mkdir(include_dir, 0700) != 0 || mkdir(src_dir, 0700) != 0) {
        perror("mkdir");
        ok = 0;
        goto cleanup;
    }

    FILE* makefile = fopen(makefile_path, "w");
    if (!makefile) {
        perror("fopen Makefile");
        ok = 0;
        goto cleanup;
    }

    fprintf(makefile,
            "SRC_DIR = src\n"
            "BUILD_DIR = build\n"
            "SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/command_processor.c\n"
            "OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/command_processor.o\n");
    fclose(makefile);

    if (chdir(temp_dir) != 0) {
        perror("chdir temp_dir");
        ok = 0;
        goto cleanup;
    }

    char* response = process_command("create c module auto_mod_test");
    if (!response) {
        fprintf(stderr, "process_command(create c module) returned NULL\n");
        ok = 0;
    } else {
        if (strstr(response, "updated Makefile") == NULL) {
            fprintf(stderr, "Expected Makefile update message, got: %s\n", response);
            ok = 0;
        }
        free(response);
    }

    if (access(header_path, F_OK) != 0) {
        fprintf(stderr, "Expected scaffold header missing: %s\n", header_path);
        ok = 0;
    }

    if (access(source_path, F_OK) != 0) {
        fprintf(stderr, "Expected scaffold source missing: %s\n", source_path);
        ok = 0;
    }

    if (!file_contains(makefile_path, "$(SRC_DIR)/auto_mod_test.c")) {
        fprintf(stderr, "Makefile missing source update for module\n");
        ok = 0;
    }

    if (!file_contains(makefile_path, "$(BUILD_DIR)/auto_mod_test.o")) {
        fprintf(stderr, "Makefile missing object update for module\n");
        ok = 0;
    }

cleanup:
    if (chdir(original_cwd) != 0) {
        perror("chdir original_cwd");
        ok = 0;
    }

    remove(header_path);
    remove(source_path);
    remove(makefile_path);
    rmdir(include_dir);
    rmdir(src_dir);
    rmdir(temp_dir);

    return ok;
}

static int test_daily_status_non_git_dir(void) {
    char original_cwd[PATH_MAX];
    if (!getcwd(original_cwd, sizeof(original_cwd))) {
        perror("getcwd");
        return 0;
    }

    char template[] = "/tmp/jarvis_daily_status_test_XXXXXX";
    char* temp_dir = mkdtemp(template);
    if (!temp_dir) {
        perror("mkdtemp");
        return 0;
    }

    int ok = 1;
    if (chdir(temp_dir) != 0) {
        perror("chdir temp_dir");
        ok = 0;
        goto cleanup;
    }

    char* response = process_command("daily status");
    if (!response) {
        fprintf(stderr, "process_command(daily status) returned NULL\n");
        ok = 0;
    } else {
        if (strstr(response, "Not a git repository.") == NULL) {
            fprintf(stderr, "Unexpected daily status response: %s\n", response);
            ok = 0;
        }
        free(response);
    }

cleanup:
    if (chdir(original_cwd) != 0) {
        perror("chdir original_cwd");
        ok = 0;
    }
    rmdir(temp_dir);

    return ok;
}

static int test_find_function_path(void) {
    char* response = process_command("find function process_command");
    if (!response) {
        fprintf(stderr, "process_command(find function) returned NULL\n");
        return 0;
    }

    int ok = 1;
    if (strstr(response, "process_command") == NULL) {
        fprintf(stderr, "Expected function search to mention process_command, got: %s\n", response);
        ok = 0;
    }
    if (strstr(response, "No matches found for") != NULL) {
        fprintf(stderr, "Expected at least one function match, got: %s\n", response);
        ok = 0;
    }

    free(response);
    return ok;
}

static int test_warning_check_flow(void) {
    char original_cwd[PATH_MAX];
    if (!getcwd(original_cwd, sizeof(original_cwd))) {
        perror("getcwd");
        return 0;
    }

    char template[] = "/tmp/jarvis_warning_test_XXXXXX";
    char* temp_dir = mkdtemp(template);
    if (!temp_dir) {
        perror("mkdtemp");
        return 0;
    }

    int ok = 1;
    char makefile_path[PATH_MAX];
    snprintf(makefile_path, sizeof(makefile_path), "%s/Makefile", temp_dir);

    FILE* makefile = fopen(makefile_path, "w");
    if (!makefile) {
        perror("fopen Makefile");
        ok = 0;
        goto cleanup;
    }

    fprintf(makefile,
            "all:\n"
            "\t@echo \"warning: synthetic warning from test\"\n\n"
            "clean:\n"
            "\t@echo \"clean\"\n");
    fclose(makefile);

    if (chdir(temp_dir) != 0) {
        perror("chdir temp_dir");
        ok = 0;
        goto cleanup;
    }

    char* response = process_command("check warnings");
    if (!response) {
        fprintf(stderr, "process_command(check warnings) returned NULL\n");
        ok = 0;
    } else {
        if (strstr(response, "Compiler warnings:") == NULL ||
            strstr(response, "synthetic warning from test") == NULL) {
            fprintf(stderr, "Unexpected warning-check response: %s\n", response);
            ok = 0;
        }
        free(response);
    }

cleanup:
    if (chdir(original_cwd) != 0) {
        perror("chdir original_cwd");
        ok = 0;
    }

    remove(makefile_path);
    rmdir(temp_dir);

    return ok;
}

static int test_open_vscode_command_path(void) {
    setenv("JARVIS_NO_GUI", "1", 1);
    char* response = process_command("open vs code");
    unsetenv("JARVIS_NO_GUI");

    if (!response) {
        fprintf(stderr, "process_command(open vs code) returned NULL\n");
        return 0;
    }

    int ok = 1;
    if (strstr(response, "Opening Visual Studio Code for you") == NULL) {
        fprintf(stderr, "Unexpected VS Code open response: %s\n", response);
        ok = 0;
    }

    free(response);
    return ok;
}

static int test_open_xcode_routes_correctly(void) {
    setenv("JARVIS_NO_GUI", "1", 1);
    char* response = process_command("open xcode");
    unsetenv("JARVIS_NO_GUI");

    if (!response) {
        fprintf(stderr, "process_command(open xcode) returned NULL\n");
        return 0;
    }

    int ok = 1;
    if (strstr(response, "Opening Xcode for you") == NULL) {
        fprintf(stderr, "Unexpected Xcode open response: %s\n", response);
        ok = 0;
    }
    if (strstr(response, "Visual Studio Code") != NULL) {
        fprintf(stderr, "Xcode command incorrectly routed to VS Code: %s\n", response);
        ok = 0;
    }

    free(response);
    return ok;
}

static int test_ai_project_bootstrap_python(void) {
    char original_cwd[PATH_MAX];
    if (!getcwd(original_cwd, sizeof(original_cwd))) {
        perror("getcwd");
        return 0;
    }

    char template[] = "/tmp/jarvis_bootstrap_test_XXXXXX";
    char* temp_dir = mkdtemp(template);
    if (!temp_dir) {
        perror("mkdtemp");
        return 0;
    }

    int ok = 1;
    char project_dir[PATH_MAX];
    char entry_path[PATH_MAX];
    char readme_path[PATH_MAX];

    snprintf(project_dir, sizeof(project_dir), "%s/auto_py", temp_dir);
    snprintf(entry_path, sizeof(entry_path), "%s/main.py", project_dir);
    snprintf(readme_path, sizeof(readme_path), "%s/README.md", project_dir);

    if (chdir(temp_dir) != 0) {
        perror("chdir temp_dir");
        ok = 0;
        goto cleanup;
    }

    setenv("JARVIS_NO_GUI", "1", 1);
    char* response = process_command("create project auto_py in python using vscode");
    unsetenv("JARVIS_NO_GUI");

    if (!response) {
        fprintf(stderr, "process_command(create project) returned NULL\n");
        ok = 0;
    } else {
        if (strstr(response, "Created python project 'auto_py'") == NULL ||
            strstr(response, "main.py") == NULL) {
            fprintf(stderr, "Unexpected project bootstrap response: %s\n", response);
            ok = 0;
        }
        free(response);
    }

    if (!is_directory_path(project_dir)) {
        fprintf(stderr, "Expected project directory missing: %s\n", project_dir);
        ok = 0;
    }

    if (access(entry_path, F_OK) != 0) {
        fprintf(stderr, "Expected project entry file missing: %s\n", entry_path);
        ok = 0;
    }

    if (access(readme_path, F_OK) != 0) {
        fprintf(stderr, "Expected project README missing: %s\n", readme_path);
        ok = 0;
    }

    if (!file_contains(entry_path, "Hello from JARVIS")) {
        fprintf(stderr, "Starter code not found in %s\n", entry_path);
        ok = 0;
    }

cleanup:
    if (chdir(original_cwd) != 0) {
        perror("chdir original_cwd");
        ok = 0;
    }

    remove(entry_path);
    remove(readme_path);
    rmdir(project_dir);
    rmdir(temp_dir);

    return ok;
}

int main(void) {
    int total = 0;
    int passed = 0;

#define RUN_TEST(fn) \
    do { \
        total++; \
        if (fn()) { \
            passed++; \
            printf("[PASS] %s\n", #fn); \
        } else { \
            printf("[FAIL] %s\n", #fn); \
        } \
    } while (0)

    RUN_TEST(test_to_lowercase);
    RUN_TEST(test_command_contains);
    RUN_TEST(test_extract_search_query);
    RUN_TEST(test_process_help);
    RUN_TEST(test_daily_status_non_git_dir);
    RUN_TEST(test_find_function_path);
    RUN_TEST(test_warning_check_flow);
    RUN_TEST(test_open_vscode_command_path);
    RUN_TEST(test_open_xcode_routes_correctly);
    RUN_TEST(test_ai_project_bootstrap_python);
    RUN_TEST(test_create_module_updates_makefile);

#undef RUN_TEST

    printf("\n%d/%d tests passed\n", passed, total);

    return (passed == total) ? 0 : 1;
}
