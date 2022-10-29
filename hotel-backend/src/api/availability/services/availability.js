'use strict';

/**
 * availability service
 */

const { createCoreService } = require('@strapi/strapi').factories;

module.exports = createCoreService('api::availability.availability');
