#!/usr/bin/env python3
"""
Automatically generates a snippets index for the cpp-snippets README.md file.
This script scans the directory structure and creates a markdown-formatted index
of all sub-repositories in the project.
"""

import os
import re
from pathlib import Path
from collections import defaultdict

# Base directory of the repository
BASE_DIR = Path(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
README_PATH = BASE_DIR / "README.md"

# Main categories to look for
CATEGORIES = [
    "algorithms",
    "concurrency",
    "data-structures",
    "design-patterns",
    "utilities",
    "programming-paradigms",
    "headers",
    "odds-and-ends",
    "tooling"
]

# Design patterns subcategories
DESIGN_PATTERN_TYPES = ["creational", "structural", "behavioral", "architectural"]

def read_readme(path):
    """Read the README file for a snippet to extract its description."""
    try:
        with open(path, 'r') as f:
            content = f.read()

            # Try to extract the first non-empty line after the title
            match = re.search(r'^# .+?\n\s*\n(.+?)$', content, re.MULTILINE)
            if match:
                # Get the captured text and remove any markdown headers
                description = match.group(1).strip()
                if description.startswith('##'):
                    # If it starts with a header, skip it and look for next content
                    content_lines = [line.strip() for line in content.split('\n')
                                   if line.strip() and not line.startswith('#')]
                    if content_lines:
                        return content_lines[0]
                    return ""
                return description

            # If no match, try to get the first non-empty line that's not a header
            content_lines = [line.strip() for line in content.split('\n')
                           if line.strip() and not line.startswith('#')]
            if content_lines:
                return content_lines[0]
            return ""
    except Exception as e:
        print(f"Error reading {path}: {e}")
        return ""

def get_snippet_description(snippet_path):
    """Get description from the README of a snippet."""
    readme_path = snippet_path / "README.md"
    if readme_path.exists():
        return read_readme(readme_path)
    return ""

def generate_index():
    """Generate the snippets index."""
    snippets = defaultdict(list)

    # Process design patterns separately due to nested structure
    design_patterns = defaultdict(list)

    # Scan all categories
    for category in CATEGORIES:
        category_dir = BASE_DIR / category
        if not category_dir.is_dir():
            continue

        # Special handling for design patterns
        if category == "design-patterns":
            for pattern_type in DESIGN_PATTERN_TYPES:
                type_dir = category_dir / pattern_type
                if not type_dir.is_dir():
                    continue

                for snippet_dir in type_dir.iterdir():
                    if snippet_dir.is_dir() and (snippet_dir / "CMakeLists.txt").exists():
                        desc = get_snippet_description(snippet_dir)
                        rel_path = snippet_dir.relative_to(BASE_DIR)
                        design_patterns[pattern_type].append((snippet_dir.name, str(rel_path), desc))
        else:
            # Process regular category
            for snippet_dir in category_dir.iterdir():
                if snippet_dir.is_dir() and (snippet_dir / "CMakeLists.txt").exists():
                    desc = get_snippet_description(snippet_dir)
                    rel_path = snippet_dir.relative_to(BASE_DIR)
                    snippets[category].append((snippet_dir.name, str(rel_path), desc))

    # Generate markdown
    markdown = []
    markdown.append("## Snippets Index")
    markdown.append("")

    # Generate category sections
    for category in CATEGORIES:
        if category == "design-patterns":
            markdown.append(f"### Design Patterns")
            markdown.append("")

            for pattern_type in DESIGN_PATTERN_TYPES:
                if design_patterns[pattern_type]:
                    markdown.append(f"#### {pattern_type.capitalize()}")
                    for name, path, desc in sorted(design_patterns[pattern_type]):
                        if desc:
                            markdown.append(f"- [{name}/]({path}/) - {desc}")
                        else:
                            markdown.append(f"- [{name}/]({path}/)")
                    markdown.append("")
        elif snippets[category]:
            category_title = " ".join(word.capitalize() for word in category.split('-'))
            markdown.append(f"### {category_title}")
            for name, path, desc in sorted(snippets[category]):
                if desc:
                    markdown.append(f"- [{name}/]({path}/) - {desc}")
                else:
                    markdown.append(f"- [{name}/]({path}/)")
            markdown.append("")

    return "\n".join(markdown)

def update_readme(index_content):
    """Update the README.md file with the generated index."""
    try:
        with open(README_PATH, 'r') as f:
            content = f.read()

        # Find the snippets index section
        start_pattern = r"## Snippets Index"
        end_pattern = r"## Contributing"

        # Replace the existing index with the new one
        new_content = re.sub(
            f"{start_pattern}.*?{end_pattern}",
            f"{index_content}\n## Contributing",
            content,
            flags=re.DOTALL
        )

        with open(README_PATH, 'w') as f:
            f.write(new_content)

        return True
    except Exception as e:
        print(f"Error updating README: {e}")
        return False

def main():
    """Main function."""
    print("Generating snippets index...")
    index_content = generate_index()

    # Option to update README directly
    choice = input("Do you want to update the README.md file directly? (y/n): ")
    if choice.lower() == 'y':
        if update_readme(index_content):
            print("README.md updated successfully!")
        else:
            print("Failed to update README.md. Printing index content instead:")
            print("\n" + index_content)
    else:
        print("\nGenerated index content:")
        print("\n" + index_content)

if __name__ == "__main__":
    main()
