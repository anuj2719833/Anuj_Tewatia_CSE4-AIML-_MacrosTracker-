$w.onReady(function () {

    // Write your Javascript code here using the Velo framework API

    // Print hello world:
    // console.log("Hello world!");

    // Call functions on page elements, e.g.:
    // $w("#button1").label = "Click me!";

    // Click "Run", or Preview your site, to execute your code

}); // Velo Code for Nutrient Calculator
// Velo API Reference: https://www.wix.com/velo/reference/api-overview/introduction
// Make sure wix-fetch is available
// Make sure wix-fetch is available
// ========== Full Nutrition Calculator ==========

// Nutrient data per 100g
import { fetch } from 'wix-fetch';

$w.onReady(() => {
    $w("#searchButton").onClick(() => {
        calculateNutrition();
    });
});

// Local nutrient data per 100g
const nutritionData = {
    "Banana": {
        calories: 89,
        protein: 1.1,
        carbs: 22.8,
        fat: 0.3
    },
    "Apple": {
        calories: 52,
        protein: 0.3,
        carbs: 13.8,
        fat: 0.2
    },
    "Rice (Cooked)": {
        calories: 130,
        protein: 2.7,
        carbs: 28.2,
        fat: 0.3
    },
    "Bread": {
        calories: 265,
        protein: 9.0,
        carbs: 49.0,
        fat: 3.2
    },
    "Chicken Breast": {
        calories: 165,
        protein: 31.0,
        carbs: 0.0,
        fat: 3.6
    }
};

async function calculateNutrition() {
    const selectedFood = $w("#foodDropdown").value;
    const grams = Number($w("#gramInput").value);

    // Validate user input
    if (!selectedFood) {
        console.log("No food selected.");
        $w("#outputText").text = "Please select a food from the dropdown.";
        return;
    }

    if (!grams || isNaN(grams) || grams <= 0) {
        console.log("Invalid grams input:", grams);
        $w("#outputText").text = "Please enter a valid number of grams.";
        return;
    }

    // Log the selected food and grams for debugging
    console.log("Selected Food:", selectedFood);
    console.log("Grams Input:", grams);

    // Check if the food is available in local nutritionData
    const foodInfo = nutritionData[selectedFood];
    if (foodInfo) {
        // Calculate from local data
        console.log("Local data found for", selectedFood);

        const multiplier = grams / 100;
        const calories = (foodInfo.calories * multiplier).toFixed(1);
        const protein = (foodInfo.protein * multiplier).toFixed(1);
        const carbs = (foodInfo.carbs * multiplier).toFixed(1);
        const fat = (foodInfo.fat * multiplier).toFixed(1);

        const result = `For ${grams}g of ${selectedFood}:\n\n` +
                       `Calories: ${calories} kcal\n` +
                       `Protein: ${protein} g\n` +
                       `Carbohydrates: ${carbs} g\n` +
                       `Fat: ${fat} g`;

        console.log("Calculated Result from Local Data:", result);

        $w("#outputText").text = result;
    } else {
        // Fallback to API if not found in local data
        console.log("No local data found for", selectedFood);
        const apiUrl = `https://world.openfoodfacts.org/cgi/search.pl?search_terms=${encodeURIComponent(selectedFood)}&search_simple=1&action=process&json=1`;

        try {
            console.log("Sending API request to:", apiUrl);
            const response = await fetch(apiUrl, { method: "get" });
            const data = await response.json();

            // Log the API response data for debugging
            console.log("API Response Data:", data);

            if (!data.products || data.products.length === 0) {
                console.log("No results found for this food.");
                $w("#outputText").text = "No results found for this food.";
                return;
            }

            const product = data.products[0];
            console.log("Fetched Product:", product);

            const nutrients = product.nutriments || {};
            const multiplier = grams / 100;

            const calories = getNutrient(nutrients, ["energy-kcal_100g", "energy_100g"]) * multiplier;
            const protein = (nutrients.proteins_100g || 0) * multiplier;
            const carbs = (nutrients.carbohydrates_100g || 0) * multiplier;
            const fat = (nutrients.fat_100g || 0) * multiplier;

            // Log calculated results for debugging
            console.log("Calculated Nutrients from API:");
            console.log("Calories:", calories, "Protein:", protein, "Carbs:", carbs, "Fat:", fat);

            // Fallback: All values are zero
            if (calories === 0 && protein === 0 && carbs === 0 && fat === 0) {
                console.log("No nutrition data available for", product.product_name || selectedFood);
                $w("#outputText").text = `No nutrition data available for ${product.product_name || selectedFood}. Try another item.`;
                return;
            }

            const resultText = `For ${grams}g of ${product.product_name || selectedFood}:\n\n` +
                               `Calories: ${calories.toFixed(1)} kcal\n` +
                               `Protein: ${protein.toFixed(1)} g\n` +
                               `Carbohydrates: ${carbs.toFixed(1)} g\n` +
                               `Fat: ${fat.toFixed(1)} g`;

            console.log("Final Result:", resultText);
            $w("#outputText").text = resultText;

        } catch (error) {
            console.error("Fetch error:", error);
            $w("#outputText").text = "Something went wrong. Please try again later.";
        }
    }
}

function getNutrient(nutrients, keys) {
    for (let key of keys) {
        if (nutrients[key]) return nutrients[key];
    }
    return 0;
}

