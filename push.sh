#!/bin/bash

# Ensure a commit message is provided
if [ -z "$1" ]; then
  echo -e "\033[31mError: Commit message is required as the first argument.\033[0m"
  echo "Usage: ./push.sh \"Your commit message\""
  exit 1
fi

# Ensure git is installed
if ! command -v git &> /dev/null; then
  echo "Error: Git is not installed. Please install it and try again."
  exit 1
fi

# Get your current Git committer email
CURRENT_EMAIL=$(git config user.email)
echo "Current committer email: $CURRENT_EMAIL"

# Function to clean the school email
clean_email() {
  local email=$1
  # Replace any local part before .42wolfsburg.de with 'student'
  echo "$email" | sed -E 's/^([^@]+)@[^@]+\.42wolfsburg\.de$/\1@student.42wolfsburg.de/'
}

# Determine new committer email
if [[ "$CURRENT_EMAIL" == *@*.42wolfsburg.de ]]; then
  NEW_EMAIL=$(clean_email "$CURRENT_EMAIL")
else
  NEW_EMAIL="$CURRENT_EMAIL"  # leave as is for gmail, etc.
fi
 echo RED "new email: $NEW_EMAIL, old email: $CURRENT_EMAIL" RE
# Add changes
echo "Adding changes to staging..."
git add .

# Commit with the correct committer email
echo "Committing with committer email: $NEW_EMAIL"
GIT_COMMITTER_EMAIL="$NEW_EMAIL" git commit -m "$1"

# Push changes
echo "Pushing changes..."
git push

echo "Done! Commit pushed with committer email: $NEW_EMAIL"