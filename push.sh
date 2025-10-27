#!/bin/bash

# Ensure a commit message is provided
if [ -z "$1" ]; then
  echo -e "\033[31mError: Commit message is required as the first argument.\033[0m"
  echo "Usage: ./push.sh \"Your commit message\""
  exit 1
fi

# Function to clean the email (replace random numbers/letters with 'student')
clean_email() {
  local email=$1
  # Replace any part of the email before '.42wolfsburg.de' with 'student'
  echo "$email" | sed -E 's/^([^@]+)@[^@]+\.42wolfsburg\.de$/\1@student.42wolfsburg.de/'
}

# Ensure git is installed
if ! command -v git &> /dev/null; then
  echo "Error: Git is not installed. Please install it and try again."
  exit 1
fi

# Get the current commit author email
CURRENT_AUTHOR_EMAIL=$(git log -1 --pretty=format:'%ae')
echo "CURRENT AUTHOR: $CURRENT_AUTHOR_EMAIL"

# Modify the current author email to replace the random part with 'student'
NEW_AUTHOR_EMAIL=$(clean_email "$CURRENT_AUTHOR_EMAIL")

# Add changes to staging
echo "Adding changes to staging..."
git add .

# Commit with the new author email
echo "Committing with author email: $NEW_AUTHOR_EMAIL"
GIT_AUTHOR_EMAIL="$NEW_AUTHOR_EMAIL" git commit -m "$1"

# Push changes
echo "Pushing changes to the repository..."
git push

# Confirmation message
echo "Done! Changes have been pushed with author email: $NEW_AUTHOR_EMAIL"

