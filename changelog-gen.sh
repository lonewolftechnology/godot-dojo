#!/bin/bash

# Configure your repository
OWNER="lonewolftechnology"
REPO="godot-dojo"

CHANGELOG_FILE="CHANGELOG.md"

# Clear the changelog file if it exists
> "$CHANGELOG_FILE"

# Get all sorted tags
TAGS=($(git tag --sort=v:refname))

# Iterate over the tags in reverse order to generate the changelog between each pair
for i in $(seq $((${#TAGS[@]} - 2)) -1 0); do
  PREVIOUS_TAG=${TAGS[$i]}
  CURRENT_TAG=${TAGS[$i+1]}

  echo "Generating changelog for ${PREVIOUS_TAG}..${CURRENT_TAG}"

  # Get the date of the current tag
  TAG_DATE=$(git log -1 --format=%as "${CURRENT_TAG}")

  # Write the version header
  echo "## [${CURRENT_TAG}](https://github.com/${OWNER}/${REPO}/tree/${CURRENT_TAG}) (${TAG_DATE})" >> "$CHANGELOG_FILE"

  # Add the list of commits using git log
  echo "**Commits:**" >> "$CHANGELOG_FILE"
  git log --pretty=format:"- [[%h](https://github.com/${OWNER}/${REPO}/commit/%H)] %s" "${PREVIOUS_TAG}"..."${CURRENT_TAG}" >> "$CHANGELOG_FILE"
  echo -e "\n" >> "$CHANGELOG_FILE"

done

echo "Changelog complete generated in ${CHANGELOG_FILE}"
