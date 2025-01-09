#!/bin/bash

# Ensure a commit message is provided
if [ -z "$1" ]; then
  echo -e "\033[33mError: Commit message is required as the first argument.\033[0m"
  echo "Usage: ./push.sh \"Your commit message\""
  exit 1
fi

# Function to clean the email (remove numbers and replace with 'student')
clean_email() {
  local email=$1
  # Replace any digits before '@' with 'student'
  echo "${email//[0-9]/student}"
}

# Set the desired author email
NEW_AUTHOR_EMAIL="dyarkovs@student.42wolfsburg.de"

# Ensure git is installed
if ! command -v git &> /dev/null; then
  echo "Error: Git is not installed. Please install it and try again."
  exit 1
fi

# Add changes to staging
echo "Adding changes to staging..."
git add .

# Commit with the specified author email
echo "Committing with author email: $NEW_AUTHOR_EMAIL"
GIT_AUTHOR_EMAIL="$NEW_AUTHOR_EMAIL" git commit -m "$1"

# Push changes
echo "Pushing changes to the repository..."
git push

# Confirmation message
echo "Done! Changes have been pushed with author email: $NEW_AUTHOR_EMAIL."
